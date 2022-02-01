#pragma once
#include "include/hiredis/hiredis.h"
#define NO_QFORKIMPL
#include "include/hiredis/win32_fixes.h"
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"
#include <boost/shared_ptr.hpp>
typedef boost::unique_lock<boost::mutex> Lock;
typedef boost::shared_ptr<redisContext> RedisContextPtr;

class HiredisManager
{
    static HiredisManager* Instance() {
        static boost::mutex mutex;
        if (!instance_) {
            Lock locker(mutex);
            if (!instance_) {
                instance_ = new HiredisManager();
            }
        }
        return instance_;
    }


    bool initRedis( std::string  hostname = "127.0.0.1",  int port = 6379 ,  int timeout = 3);
private:
    static HiredisManager* instance_;
    RedisContextPtr        redisClient;
private:
    HiredisManager();
};

