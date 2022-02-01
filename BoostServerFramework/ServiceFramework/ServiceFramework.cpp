#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include "NetworkManager/network_manager.h"
#include "RegisterHandler.h"
#include "TestMethod.h"
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "MysqlManager/dbio_mysql.hpp"
#include "Hiredis/redis_manager.h"
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

bool SelectSysSQL(const boost::shared_ptr<DBIO> & sysdbio_,const std::string& strSQL, std::vector<std::vector<std::string>>& vecRes)
{
	std::string 		query = strSQL;

	vecRes.clear();
	if (!sysdbio_)
		return false;

	long ret_code = SqlRetTypeNormal;
	vecRes = sysdbio_->selectData(query, ret_code);

	return (ret_code == SqlRetTypeNormal);
}

bool InsertSysSQL(const boost::shared_ptr<DBIO>& pDBIO,const std::string& strSQL)
{
	if (pDBIO == NULL)
		return false;

	std::string 		query = strSQL;

	return pDBIO->insertData(query);

}
bool UpdateSysSQL(const boost::shared_ptr<DBIO>& pDBIO, const std::string& strSQL)
{

	if (pDBIO == NULL)
		return false;

	std::string 		query = strSQL;

	return pDBIO->updateData(query);

}
bool DeleteSysSQL(const boost::shared_ptr<DBIO>& pDBIO, const std::string& strSQL)
{

	if (pDBIO == NULL)
		return false;

	std::string 		query = strSQL;

	return pDBIO->deleteData(query);
}
int main()
{
	//------------------------mysql------------------------------------------
	boost::shared_ptr<DBIO> pDBIO = NULL;

	pDBIO.reset(new DBIOMysql());

	bool ret = pDBIO->initConnectionPool("dbserver", "dbuser", "dbpassword", "strDBName", 20/*pool size*/, 7777/*port*/);
	if (ret)
	{
		std::cout << "mysql init success";
		//std::vector<std::vector<std::string>> ret;
		//SelectSysSQL(pDBIO,"", ret);
		//InsertSysSQL(pDBIO,"");
		//UpdateSysSQL(pDBIO, "");
		//DeleteSysSQL(pDBIO, "");
	}
	else {
		std::cout << "mysql init error";

	}
	//------------------------redis初始化------------------------------------------
	RedisManager::Instance()->initRedis();
	//------------------------方法注册------------------------------------------
	
	HandlerManager::Instance().InitHandler<TestMethod>(1);
	//------------------------网络初始化------------------------------------------

	NetworkManager::Instance()->tcpAcceptor(66666);//监听  (登录后再将客户端连接加到连接池里)

	//------------------------启动------------------------------------------

	IOManager::instance().run();//启动服务work


	return 0;
}