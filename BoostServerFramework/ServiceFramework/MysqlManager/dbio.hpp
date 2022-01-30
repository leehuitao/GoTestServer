#pragma once
#include <vector>
#include <cstring>
#include <map>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>
#include "../NetworkManager/io_manager.h"

using namespace std;

/** 数据库类型枚举 */
enum DatabaseType {
    kDatabaseTypeMysql = 0,     /**< mysql */
    kDatabaseTypeOracle,    /**< oracle */
    kDatabaseTypeMSSql,     /**< sql server */
    kDatabaseTypeOdbc       /**< odbc */
};

/** 执行数据库语句返回错误代码 */
enum ExecSqlRetType {
	SqlRetTypeNormal = 0,     /* 正常返回 */
	SqlRetTypeGetConnPtrError,     /* 获取连接失败 */
	SqlRetTypePingError,     /*ping 失败 */
	SqlRetTypeSetAutoCommitError,       /**<  */  
	SqlRetTypeTableNotExistError,   /* 金仓为5，普通mysql为4*/
	SqlRetTypeSetRealQueryError	,	/* */
	SqlRetTypeSetSchemaError,
	SqlRetTypeOtherError
};



struct ConnectionWrapBase
{

};

typedef boost::shared_ptr<ConnectionWrapBase> ConnectionWrapBasePtr;
/**
 * @brief
 */
class DBIO : public boost::enable_shared_from_this<DBIO>
{
public:
	explicit DBIO() : dbtype_(kDatabaseTypeMysql),
					  db_ioservice_(IOManager::instance().ioService()),
					  db_timer_(db_ioservice_)
	{
	}
	virtual ~DBIO(){}

	virtual bool initConnectionPool(const std::string& serverAddr,
	        const std::string& username,
	        const std::string& passwd,
	        const std::string& dbname,
	        const std::size_t& maxSize,
			const std::size_t& port) = 0;
	virtual void close() = 0;
	virtual std::vector<std::vector<std::string> > selectData(const std::string& query) = 0;
	virtual std::vector<std::vector<std::string> > selectData(const std::string& query, long &ret_code) = 0;
	virtual std::vector<std::string> selectDataRow(const std::string& query) = 0;
	virtual bool insertData(const std::string& query) = 0;
	virtual bool deleteData(const std::string& query) = 0;
	virtual bool insertData(const std::string& query, long &ret_code) = 0;
	virtual bool updateData(const std::string& query) = 0;
	virtual bool execData(const std::string& query, ConnectionWrapBasePtr conn, long &ret_code) = 0;

	 int getDbType() const {return dbtype_;}

	virtual ConnectionWrapBasePtr connection() = 0;
	virtual void releaseConnection(ConnectionWrapBasePtr ptrConn) = 0;

	virtual void checkDbConnections() = 0;

    virtual void startCheckConnection(const boost::system::error_code& ec)  {
    	if (ec) {
    		return;
    	}
        this->checkDbConnections();
        db_timer_.expires_from_now(boost::posix_time::seconds(30/*4 * 60 * 60*/));
        db_timer_.async_wait(boost::bind(&DBIO::startCheckConnection, shared_from_this(), boost::asio::placeholders::error));
    }

    virtual void stopCheckConnection(){
        boost::system::error_code ec;
        db_timer_.cancel(ec);
        db_timer_.expires_at(boost::posix_time::pos_infin, ec);
    }

	virtual bool beginTransaction(ConnectionWrapBasePtr connPtr) = 0;
	virtual bool commitTransaction(ConnectionWrapBasePtr connPtr) = 0;
	virtual bool rollBack(ConnectionWrapBasePtr connPtr) = 0;

private:
	int dbtype_;

    boost::asio::io_service& db_ioservice_;  /**< 连接池所需io */
    boost::asio::deadline_timer db_timer_;   /**< 连接检查器 */
};

