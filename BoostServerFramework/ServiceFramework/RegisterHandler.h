#pragma once

#include "NetworkManager/message_protocol.h"
#include "NetworkManager/msghandler_repository.h"
#include "NetworkManager/tcp_connection.h"
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"

typedef boost::unique_lock<boost::mutex> Lock;

class RegisterHandler
{

public:
    static RegisterHandler* Instance() {
        static boost::mutex mutex;
        if (!instance_) {
            Lock locker(mutex);
            if (!instance_) {
                instance_ = new RegisterHandler();
            }
        }
        return instance_;
    }
		bool InitHandler(unsigned short cmd, RequestHandler handler);
private:
    static RegisterHandler* instance_;
};

