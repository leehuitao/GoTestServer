#pragma once
#include <iostream>
#include <string>
#include <boost/format.hpp>
#ifdef WIN32
#include <windows.h>
#endif
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include "connection_pool_abstract.hpp"
using namespace std;

/**
 * @brief MySql连接池类
 */
class MySqlConnectionPool: public ConnectionPoolAbstract<MYSQL>
{
public:

	explicit MySqlConnectionPool(){}
    void setParam(const std::string& serverAddr,
			const std::size_t& port,
            const std::string& username,
            const std::string& passwd,
            const std::string& dbname,
            const std::size_t& maxSize);

	virtual ConnectionPtr connection() {
		return ConnectionPtr();
	}
	virtual ConnectionPtr createConnection() {
		return ConnectionPtr();
	}
    virtual bool initConnection();
	virtual void releaseConnection(ConnectionPtr ptrConn) {}
    virtual void checkDbConnections();
    virtual void close();

	virtual bool beginTransaction(ConnectionPtr connPtr) { return false; }
	virtual bool commitTransaction(ConnectionPtr connPtr) { return false; }
	virtual bool rollBack(ConnectionPtr connPtr){ return false;  }

    virtual bool beginTransaction(Connection* connPtr);
    virtual bool commitTransaction(Connection* connPtr);
    virtual bool rollBack(Connection* connPtr);
	virtual Connection* mySqlCreateConnection();
	virtual Connection* mySqlconnection();
	virtual void releaseConnection(Connection* connPtr);
private:
    std::string _serverAddr;    /**< mysql服务器的ip */
    std::string _username;      /**< mysql服务器的用户名 */
    std::string _passwd;        /**< mysql服务器的密码 */
    std::string _dbname;        /**< 需要使用的数据库名 */
    std::size_t _port;

};
