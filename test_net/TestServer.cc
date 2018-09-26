#include <unistd.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <set>
#include <map>

#include "net/Connection.h"
#include "net/Application.h"
#include "util/Logger.h"



using ananas::Connection;

std::shared_ptr<ananas::Logger> logger;
std::once_flag                  m_flag;
std::mutex                      mu;






std::map<int,const Connection *>Order_groups;





ananas::PacketLen_t OnMessage(ananas::Connection* conn, const char* data, ananas::PacketLen_t len) {
    // echo package
    //data.append("[wuyujie:<response>]");
    
    std::string rsp(data, len);
	//rsp.append("[wuyujie:<response>]");
	////receive request content
	INF(logger)<<rsp;
    //conn->SendPacket(rsp.data(), rsp.size());
    return len;
}

void OnNewConnection(ananas::Connection* conn) {
    using ananas::Connection;

	INF(logger) << conn->Peer().ToString();
	{
		std::unique_lock<std::mutex>guard(mu);
		if(Order_groups.find(conn->Identifier()) == Order_groups.end())
		{
		   Order_groups.insert({conn->Identifier(),conn});
		}
		guard.unlock();
	}
	
    conn->SetOnMessage(OnMessage);
    conn->SetOnDisconnect([](Connection* conn) {
        WRN(logger) << "OnDisConnect " << conn->Identifier();
		{
			std::unique_lock<std::mutex>guard(mu);
			if(Order_groups.find(conn->Identifier()) != Order_groups.end())
			{
			   Order_groups.erase(conn->Identifier());
			}
			guard.unlock();
  	    }
	
    });
}


int main(int ac, char* av[]) {
    size_t workers = 1;
    if (ac > 1)
        workers = (size_t)std::stoi(av[1]);

	std::call_once(m_flag,[&](){
	    ananas::LogManager::Instance().Start();
	    logger = ananas::LogManager::Instance().CreateLog(logALL, logConsole);
	});
	
    

    auto& app = ananas::Application::Instance();
    app.SetNumOfWorker(workers);
    app.Listen("127.0.0.1", 9987, OnNewConnection);

    app.Run(ac, av);

    return 0;
}

