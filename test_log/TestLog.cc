
#include <iostream>
#include <string>

#include "util/ThreadPool.h"
#include "util/Logger.h"
#include "util/TimeUtil.h"

int g_threads = 2;
const int kLogs = 100 * 10000;

std::atomic<int> g_complete(0);

int main(int ac, char* av[]) {

    //start LogManager
    ananas::LogManager::Instance().Start();
	auto logger=ananas::LogManager::Instance().CreateLog(logALL,logFile,"log");
	if(ac < 20)
	{
	  INF(logger)<<"wuyujie:"<<ac;
	}else{
		DBG(logger)<<"something error";
	}

	//test multi_thread

	ananas::ThreadPool pool;
	pool.SetMaxThreads(50);

	pool.Execute([&](int a,int b){
		
		  std::cout<<a+b<<std::endl;
		  INF(logger)<<"result "<<a+b;
		  
	},1,2);

	pool.Execute([&](){

      std::cout<<"a+b"<<"now time: ";
	  

	});
	
    
	
	
   
	pool.JoinAll();
	

  
	ananas::LogManager::Instance().Stop();

	
	#if 0
    if (ac > 1)
        g_threads = std::stoi(av[1]);

    if (g_threads > 50)
        g_threads = 50;
    else if (g_threads <= 0)
        g_threads = 1;

    ananas::LogManager::Instance().Start();

    ananas::ThreadPool pool;

    ananas::Time start;

    for (int i = 0; i < g_threads; ++ i) {
        auto log = ananas::LogManager::Instance().CreateLog(logALL, logFile, "logtestdir");
        pool.Execute(
        [=]() {
            for (int n = 0; n < (kLogs/ g_threads); n ++) {
                DBG(log) << n << "|abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy|";
            }

            ++ g_complete;
        }
        );
    }

    // 等待生产者线程结束
    while (g_complete < g_threads) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    ananas::LogManager::Instance().Stop();

    ananas::Time end;
    std::cout << g_threads << " threads, used " << (end - start) << "ms" << std::endl;;
    std::cout << int(kLogs * 1000.0 / (end - start) / 10000)
              << "w logs/second\n";

    pool.JoinAll();
	#endif 

    return 0;
}

