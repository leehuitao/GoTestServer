#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include "NetworkManager/network_manager.h"
#include "RegisterHandler.h"
#include "TestMethod.h"
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

class HandlerManager : private boost::noncopyable
{
public:
    static HandlerManager& Instance();

 
    template<class Handler>
    const bool InitHandler(const unsigned short cmd);
private:

    template<class Handler>
    void NotifyHandler(RequestBufferPtr msg);

private:
    bool registered_;

};

HandlerManager& HandlerManager::Instance()
{
	static HandlerManager g_instance;
	return g_instance;
}

template<class Handler>
void HandlerManager::NotifyHandler(RequestBufferPtr msg)
{
	Handler handler;
	handler.HandleMessage(msg);
}

template<class Handler>
const bool HandlerManager::InitHandler(const unsigned short cmd)
{
	RegisterHandler::Instance()->InitHandler(cmd, boost::bind(&HandlerManager::NotifyHandler<Handler>, this, _1));
	return 1;
}
int main()
{
	HandlerManager::Instance().InitHandler<TestMethod>(1);//方法注册

	NetworkManager::Instance()->tcpAcceptor(66666);//监听  (登录后再将客户端连接加到连接池里)

	IOManager::instance().run();//启动服务work
	
	return 0;
}