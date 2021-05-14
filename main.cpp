#include <iostream>
#include <signal.h>

#ifdef _WIN32

#include <windows.h>

#else
#include <unistd.h>
#endif

#include "RedisPool.h"

using namespace hiredis;
static bool finished = false;

static void signalHandler(int signo) {
    std::cerr << "Shutting down" << std::endl;
    finished = true;
}

int main() {
    printf("bbbbbbbbbbbbbbbbbbbbb\n");
    //udpClient client;
    // client.Init(5060);
    //  client.sip_uac_message_register(client.sip, &client.sipTransport);
    //OXF::Instance().Initialize();
    // std::shared_ptr<GB_Server> server = std::make_shared<GB_Server>();
    // server->Init(0);
    bool running = true;
    //   if (!server->Run()) {
    //       running = false;
    //  }

    RedisPool::Instance().Init("test", "192.168.1.61", 6379, 0);
    RedisPool::Instance().GetCacheConn()->Set("123", std::string("456"));
    printf("aaaaaaaaaaaaaaaaaaaaa\n");
    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        std::cerr << "Couldn't install signal handler for SIGINT" << std::endl;
        exit(-1);
    }

    if (signal(SIGTERM, signalHandler) == SIG_ERR) {
        std::cerr << "Couldn't install signal handler for SIGTERM" << std::endl;
        exit(-1);
    }

	Sleep(10 * 1000);

    if (running) {
        while (!finished) {
#ifdef WIN32
            Sleep(5);
#else
            usleep(1000*5);
#endif
			RedisPool::Instance().GetCacheConn()->Set("123", std::string("456"));
			std::thread t1([]() {
				RedisPool::Instance().GetCacheConn()->Set("123", std::string("456"));
			});
			std::thread t2([]() {
				RedisPool::Instance().GetCacheConn()->Set("123", std::string("456"));
			});
			std::thread t3([]() {
				RedisPool::Instance().GetCacheConn()->Set("123", std::string("456"));
			});
			std::thread t4([]() {
				RedisPool::Instance().GetCacheConn()->Set("123", std::string("456"));
			});
			t1.join();
			t2.join();
			t3.join();
			t4.join();
        }
    }
    return 0;
}
