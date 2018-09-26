#include <iostream>
#include <thread>
#include <chrono>

#include "net/EventLoop.h"
#include "net/Application.h"
#include "util/Logger.h"

using namespace ananas;

std::shared_ptr<Logger> log;

int main(int ac, char* av[]) {
    LogManager::Instance().Start();
    log = LogManager::Instance().CreateLog(logALL, logConsole);

    auto& app = Application::Instance();
    auto& loop = *app.BaseLoop();

    loop.Execute([]() {
        INF(log) << "Hello, test timer...";
    });

    // shutdown after 7s
    /*
    loop.ScheduleAfter(std::chrono::seconds(7), [&app]() {
        WRN(log) << "Now stop app.";
        app.Exit();
    });
    */

	// heartbeat
	loop.ScheduleAfterWithRepeat<kForever>(std::chrono::seconds(3),[&](){
	          std::cout<<">>>>>>>>>>>>>heartbeat>>>>>>>>>>>>>"<<std::endl;
              DBG(log) <<"heartbeat.....";
			  
	});
	
   /*
    int count = 0;
    auto forever = loop.ScheduleAfterWithRepeat<kForever>(std::chrono::seconds(1), [&count]() {
        ++ count;
        DBG(log) << "count = " << count << ", you can not see me more than twice.";
    });
    int times = 0;
    auto only5 = loop.ScheduleAfterWithRepeat<5>(std::chrono::seconds(1), [=, &loop, &times]() {
        ++ times;
        DBG(log) << "Trigger every 1s, the " << times << " time";
        if (times == 2) {
            if (loop.Cancel(forever))
                WRN(log) << "Cancel timer";
            else
                ERR(log) << "BUG: Cancel failed!!!";
        }
    });
    (void)only5;
    */ 
    

    app.Run(ac, av);

    return 0;
}

