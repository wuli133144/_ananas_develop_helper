#include <unistd.h>
#include <iostream>
#include <assert.h>
#include <thread>
#include <mutex>

#include "net/Connection.h"
#include "net/EventLoop.h"
#include "net/Application.h"
#include "util/Logger.h"
#include "util/TimeUtil.h"

ananas::Time start, end;
int nowCount = 0;
const int totalCount = 200 * 10000;

std::shared_ptr<ananas::Logger> logger;
std::once_flag                  m_flag;



ananas::PacketLen_t OnMessage(ananas::Connection* conn, const char* data, size_t len) {
    /*++ nowCount;
    if (nowCount == totalCount) {
        end.Now();
        USR(logger) << "Done OnResponse avg " << (totalCount * 0.1f / (end - start)) << " W/s";
        ananas::Application::Instance().Exit();
        return 0;
    }
    if (nowCount % 100000 == 0) {
        end.Now();
        USR(logger) << "OnResponse avg " << (nowCount * 0.1f / (end - start)) << " W/s";
    }
    */
    // echo package
    std::string rsp(data, len);
	INF(logger)<<rsp;
    conn->SendPacket(rsp.data(), rsp.size());
    return len;
}

void OnWriteComplete(ananas::Connection* conn) {
    //INF(logger) << "OnWriteComplete for " << conn->Identifier();
}

void OnConnect(ananas::Connection* conn) {
    INF(logger) << "OnConnect " << conn->Identifier();

    std::string tmp = "abcdefghijklmnopqrstuvwxyz[request]";
    std::string msg = tmp ;//+ tmp + tmp + tmp;
    
    conn->SendPacket(msg.data(), msg.size());
}

void OnDisConnect(ananas::Connection* conn) {
    INF(logger) << "OnDisConnect " << conn->Identifier();
}

void OnNewConnection(ananas::Connection* conn) {
    conn->SetOnConnect(OnConnect);
    conn->SetOnMessage(OnMessage);
    conn->SetOnDisconnect(OnDisConnect);
    //conn->SetOnWriteComplete(OnWriteComplete);
}


void OnConnFail(ananas::EventLoop* loop, const ananas::SocketAddr& peer) {
    INF(logger) << "OnConnFail " << peer.GetPort();

    // reconnect
    loop->ScheduleAfter(std::chrono::seconds(2),
    [=]() {
        loop->Connect(peer,
                      OnNewConnection,
                      OnConnFail,
                      ananas::DurationMs(3000));
    });
}

void SanityCheck(int& threads) {
    if (threads < 1)
        threads = 1;
    else if (threads > 100)
        threads = 100;
}

int main(int ac, char* av[]) {

    std::call_once(m_flag,[&](){
		    ananas::LogManager::Instance().Start();
            logger = ananas::LogManager::Instance().CreateLog(logALL, logConsole);
	});


    int threads = 1;
    if (ac > 1) {
        threads = std::stoi(av[1]);
        SanityCheck(threads);
    }

    const uint16_t port = 9987;
    const int kConns = threads;

    auto& app = ananas::Application::Instance();
    app.SetNumOfWorker(threads);
	
    //for (int i = 0; i < kConns; ++ i)
    app.Connect("127.0.0.1", port, OnNewConnection, OnConnFail, ananas::DurationMs(3000));
    // app.Connect()

    start.Now();
    app.Run(ac, av);

    return 0;
}

