#pragma once
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
extern "C" {
    #include "..\..\hiredis.h"
    #include "..\..\async.h"
    #include "..\..\adapters\ae.h"
}
typedef boost::unique_lock<boost::mutex> Lock;

class RedisManager
{

public:
    static RedisManager* Instance() {
        static boost::mutex mutex;
        if (!instance_) {
            Lock locker(mutex);
            if (!instance_) {
                instance_ = new RedisManager();
            }
        }
        return instance_;
    }
    void initRedis(std::string hostname = "127.0.0.1",int port = 6379 , int poolsize = 100);
private:
    static RedisManager* instance_;
};
