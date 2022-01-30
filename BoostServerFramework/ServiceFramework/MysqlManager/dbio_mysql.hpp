#pragma once
#include "connection_pool_mysql.hpp"
#include "dbio.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

struct ConnectionWrapMysql : public ConnectionWrapBase
{
	MySqlConnectionPool::Connection* conn_;
};

typedef boost::shared_ptr<ConnectionWrapMysql> ConnectionWrapMysqlPtr;

class DBIOMysql : public DBIO
{
public:
	bool initConnectionPool(const std::string& serverAddr,
	        const std::string& username,
	        const std::string& passwd,
	        const std::string& dbname,
	        const std::size_t& maxSize,
			const std::size_t& port);

	virtual ~DBIOMysql();

	virtual void close();

	virtual std::vector<std::vector<std::string> > selectData(const std::string& query);
	virtual std::vector<std::vector<std::string> > selectData(const std::string& query, long &ret_code);
	virtual std::vector<std::string> selectDataRow(const std::string& query);
	virtual bool insertData(const std::string& query);
	virtual bool insertData(const std::string& query, long &ret_code);
	virtual bool deleteData(const std::string& query);
	virtual bool updateData(const std::string& query);
	virtual bool execData(const std::string& query, ConnectionWrapBasePtr conn, long &ret_code);
	virtual void checkDbConnections();

	ConnectionWrapBasePtr connection();
	virtual void releaseConnection(ConnectionWrapBasePtr ptrConn);

	virtual bool beginTransaction(ConnectionWrapBasePtr connPtr);
	virtual bool commitTransaction(ConnectionWrapBasePtr connPtr);
	virtual bool rollBack(ConnectionWrapBasePtr connPtr);

private:
	boost::shared_ptr<MySqlConnectionPool> pool_;

    string dbServerAddr_;
    string dbUsername_;
    string dbPasswd_;
    string dbname_;
};
