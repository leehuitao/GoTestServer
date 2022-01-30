
#include "connection_pool_mysql.hpp"
#include "dbio_mysql.hpp"

DBIOMysql::~DBIOMysql()
{
	if (pool_)
	{
		close();
	}
}

/*  */

vector<vector<string> > DBIOMysql::selectData(const string& query)
{
#if defined(DEBUG)
	cout << "DBIOMysql::selectData" << endl;
	cout << "excute sql: " << query << endl;
#endif

    std::vector<std::vector<std::string> > resVector;
    MySqlConnectionPool::Connection* connPtr = pool_->mySqlconnection();
    MYSQL_RES *res;
    MYSQL_ROW row;

    if (!connPtr) {
		//BIGANT_ERROR_TRACE_THIS("selectData connPtr is null");
        return resVector;
    }

    if (::mysql_ping(connPtr)) {
		//BIGANT_ERROR_TRACE_THIS("mysql ping  is error");
        ::mysql_close(connPtr);
        return resVector;
    }
    std::string tmpQuery("SET AUTOCOMMIT=1;");
    if (::mysql_real_query(connPtr, tmpQuery.c_str(), (unsigned int)strlen(tmpQuery.c_str()))) {
		//BIGANT_ERROR_TRACE_THIS(::mysql_error(connPtr)<< "SQL="<<tmpQuery);
        pool_->releaseConnection(connPtr);
        return resVector;
    }
    if (::mysql_real_query(connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		//BIGANT_ERROR_TRACE_THIS(::mysql_error(connPtr) << "SQL=" << query);
        pool_->releaseConnection(connPtr);
        return resVector;
    }

    res = ::mysql_store_result(connPtr);
    row = ::mysql_fetch_row(res);

    while (row) {
        std::vector<std::string> vecLine;
        for (int i = 0; i < ::mysql_num_fields(res); ++i) {
            if (row[i] == NULL) {
                vecLine.push_back(std::string());
                continue;
            }
            vecLine.push_back(std::string(row[i]));
        }
        row = ::mysql_fetch_row(res);
        resVector.push_back(vecLine);
        vecLine.clear();
        std::vector<std::string>().swap(vecLine);
    }

    ::mysql_free_result(res);
    pool_->releaseConnection(connPtr);
    return resVector;
}


vector<vector<string> > DBIOMysql::selectData(const string& query, long &ret_code)
{
#if defined(DEBUG)
	cout << "DBIOMysql::selectData" << endl;
	cout << "excute sql: " << query << endl;
#endif

	ret_code = SqlRetTypeNormal;
    std::vector<std::vector<std::string> > resVector;
	MySqlConnectionPool::Connection* connPtr = pool_->mySqlconnection();
    MYSQL_RES *res;
    MYSQL_ROW row;

    if (!connPtr) {
       // BIGANT_ERROR_TRACE("selectData connPtr is null" );
        ret_code = SqlRetTypeGetConnPtrError;
        return resVector;
    }

    if (::mysql_ping(connPtr)) {
		//BIGANT_ERROR_TRACE("mysql ping  is error");
        ret_code = SqlRetTypePingError;
        ::mysql_close(connPtr);
        return resVector;
    }
    std::string tmpQuery("SET AUTOCOMMIT=1;");
    if (::mysql_real_query(connPtr, tmpQuery.c_str(), (unsigned int)strlen(tmpQuery.c_str()))) {
		//BIGANT_ERROR_TRACE(::mysql_error(connPtr)<<" SQL="<< tmpQuery);
        pool_->releaseConnection(connPtr);
        ret_code = SqlRetTypeSetAutoCommitError;
        return resVector;
    }
    if (::mysql_real_query(connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		//BIGANT_ERROR_TRACE(::mysql_error(connPtr)<< " SQL=" << query);
        pool_->releaseConnection(connPtr);
        ret_code = SqlRetTypeSetRealQueryError;
        return resVector;
    }

    res = ::mysql_store_result(connPtr);
    row = ::mysql_fetch_row(res);

    while (row) {
        std::vector<std::string> vecLine;
        for (int i = 0; i < ::mysql_num_fields(res); ++i) {
            if (row[i] == NULL) {
                vecLine.push_back(std::string());
                continue;
            }
            vecLine.push_back(std::string(row[i]));
        }
        row = ::mysql_fetch_row(res);
        resVector.push_back(vecLine);
        vecLine.clear();
        std::vector<std::string>().swap(vecLine);
    }

    ::mysql_free_result(res);
    pool_->releaseConnection(connPtr);
    return resVector;
}


std::vector<std::string> DBIOMysql::selectDataRow(const std::string& query)
{
    std::vector<std::string> resVector;
	MySqlConnectionPool::Connection* connPtr = pool_->mySqlconnection();
    MYSQL_RES *res;
    MYSQL_ROW row;

    if (!connPtr) {
		//BIGANT_ERROR_TRACE("selectDataRow connPtr is null");
        return resVector;
    }

    if (::mysql_ping(connPtr)) {
		//BIGANT_ERROR_TRACE("mysql ping  is error");
        ::mysql_close(connPtr);
        return resVector;
    }
    std::string tmpQuery("SET AUTOCOMMIT=1;");
    if (::mysql_real_query(connPtr, tmpQuery.c_str(), (unsigned int)strlen(tmpQuery.c_str()))) {
		//BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << tmpQuery);
        pool_->releaseConnection(connPtr);
        return resVector;
    }
    if (::mysql_real_query(connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		//BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << query);
        pool_->releaseConnection(connPtr);
        return resVector;
    }

    res = ::mysql_store_result(connPtr);
    row = ::mysql_fetch_row(res);
    if (row)
    {
        for (int i = 0; i < ::mysql_num_fields(res); ++i)
        {
            resVector.push_back(std::string(row[i]));
        }
    }

    ::mysql_free_result(res);
    pool_->releaseConnection(connPtr);
    return resVector;


}

 bool DBIOMysql::insertData(const std::string& query)
 {
	 MySqlConnectionPool::Connection* connPtr = pool_->mySqlconnection();

	 if (!connPtr) {
		 //BIGANT_ERROR_TRACE("selectDataRow connPtr is null");
		 return false;
	 }
	 if (::mysql_ping(connPtr)) {
		// BIGANT_ERROR_TRACE("mysql ping  is error");
	     ::mysql_close(connPtr);
		 return false;
	 }
	 std::string tmpQuery("SET AUTOCOMMIT=1;");
	 if (::mysql_real_query(connPtr, tmpQuery.c_str(), (unsigned int)strlen(tmpQuery.c_str()))) {
		 //BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << tmpQuery);
		 pool_->releaseConnection(connPtr);
		 return false;
	 }
	 if (::mysql_real_query(connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		 //BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << query);
		 pool_->releaseConnection(connPtr);
		 return false;
	 }
	 pool_->releaseConnection(connPtr);
	 return true;
 }


 bool DBIOMysql::insertData(const std::string& query, long &ret_code)
  {
	 ret_code = SqlRetTypeNormal;
	 MySqlConnectionPool::Connection* connPtr = pool_->mySqlconnection();

 	 if (!connPtr) {
		 //BIGANT_ERROR_TRACE("selectDataRow connPtr is null");
 		 ret_code = SqlRetTypeGetConnPtrError;
 		 return false;
 	 }
 	 if (::mysql_ping(connPtr)) {
		 //BIGANT_ERROR_TRACE("mysql ping  is error");
 		 ret_code = SqlRetTypePingError;
         ::mysql_close(connPtr);
 		 return false;
 	 }
 	 std::string tmpQuery("SET AUTOCOMMIT=1;");
 	 if (::mysql_real_query(connPtr, tmpQuery.c_str(), (unsigned int)strlen(tmpQuery.c_str()))) {
		 //BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << tmpQuery);
         ret_code = SqlRetTypeSetAutoCommitError;
 		 pool_->releaseConnection(connPtr);
 		 return false;
 	 }
 	 if (::mysql_real_query(connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		 //BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << query);
         if (mysql_errno(connPtr) == ER_BAD_TABLE_ERROR || mysql_errno(connPtr) == ER_NO_SUCH_TABLE)
        	 ret_code = SqlRetTypeTableNotExistError;
         else
        	 ret_code = SqlRetTypeSetRealQueryError;
 		 pool_->releaseConnection(connPtr);
 		 return false;
 	 }
 	 pool_->releaseConnection(connPtr);
 	 return true;
  }


 bool DBIOMysql::deleteData(const std::string& query)
 {
	 MySqlConnectionPool::Connection* connPtr = pool_->mySqlconnection();

	 if (!connPtr) {
		// BIGANT_ERROR_TRACE("selectDataRow connPtr is null");
		 return false;
	 }
	 if (::mysql_ping(connPtr)) {
		 //BIGANT_ERROR_TRACE("mysql ping  is error");
	     ::mysql_close(connPtr);
		 return false;
	 }
	 std::string tmpQuery("SET AUTOCOMMIT=1;");
	 if (::mysql_real_query(connPtr, tmpQuery.c_str(), (unsigned int)strlen(tmpQuery.c_str()))) {
		//BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << tmpQuery);
        pool_->releaseConnection(connPtr);
		return false;
	 }
	 if (::mysql_real_query(connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		//BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << query);
        pool_->releaseConnection(connPtr);
		return false;
	 }

	 pool_->releaseConnection(connPtr);
	 return true;
 }

 bool DBIOMysql::updateData(const std::string& query)
 {
	 MySqlConnectionPool::Connection* connPtr = pool_->mySqlconnection();

	 if (!connPtr) {
		 //BIGANT_ERROR_TRACE("selectDataRow connPtr is null");
		 return false;
	 }
	 if (::mysql_ping(connPtr)) {
		 //BIGANT_ERROR_TRACE("mysql ping  is error");
	     ::mysql_close(connPtr);
		 return false;
	 }
	 std::string tmpQuery("SET AUTOCOMMIT=1;");
	 if (::mysql_real_query(connPtr, tmpQuery.c_str(), (unsigned int)strlen(tmpQuery.c_str()))) {
		 //BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << tmpQuery);
        pool_->releaseConnection(connPtr);
		return false;
	 }
	 if (::mysql_real_query(connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		// BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << query);
        pool_->releaseConnection(connPtr);
		return false;
	 }

	 pool_->releaseConnection(connPtr);
	 return true;
 }

bool DBIOMysql::execData(const std::string& query, ConnectionWrapBasePtr conn, long &ret_code)
{
	ConnectionWrapMysqlPtr conn_mysql = boost::static_pointer_cast<ConnectionWrapMysql>(conn);
	 ret_code = SqlRetTypeNormal;
	 MySqlConnectionPool::Connection* connPtr = conn_mysql->conn_;

	 if (!connPtr) {
		 //BIGANT_ERROR_TRACE("selectDataRow connPtr is null");
		 ret_code = SqlRetTypeGetConnPtrError;
		 return false;
	 }
	 if (::mysql_ping(connPtr)) {
		//BIGANT_ERROR_TRACE("mysql ping  is error");
		ret_code = SqlRetTypePingError;
        ::mysql_close(connPtr);
		 return false;
	 }

	 if (::mysql_real_query(connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		//BIGANT_ERROR_TRACE(::mysql_error(connPtr) << " SQL=" << query);
        if (mysql_errno(connPtr) == ER_BAD_TABLE_ERROR || mysql_errno(connPtr) == ER_NO_SUCH_TABLE)
       	 ret_code = SqlRetTypeTableNotExistError;
        else
       	 ret_code = SqlRetTypeSetRealQueryError;

		 return false;
	 }

	 return true;
}

bool DBIOMysql::initConnectionPool(const std::string& serverAddr,
        								const std::string& username,
										const std::string& passwd,
										const std::string& dbname,
										const std::size_t& maxSize,
										const std::size_t& port)
{

    this->pool_.reset(new MySqlConnectionPool());
    pool_->setParam(serverAddr, port, username, passwd, dbname, maxSize);
    if (!pool_->initConnection())
    {
		 return false;
    }

    startCheckConnection(boost::system::error_code());

    return true;
}

ConnectionWrapBasePtr DBIOMysql::connection()
 {
	ConnectionWrapMysqlPtr conn(new ConnectionWrapMysql());
	conn->conn_ = pool_->mySqlconnection();
	if (!conn->conn_)
	{
		return ConnectionWrapBasePtr();
	}

	return boost::static_pointer_cast<ConnectionWrapBase>(conn);
 }

 void DBIOMysql::releaseConnection(ConnectionWrapBasePtr ptrConn)
 {
	 if (ptrConn)
	 {
		 ConnectionWrapMysqlPtr conn = boost::static_pointer_cast<ConnectionWrapMysql>(ptrConn);
		 pool_->releaseConnection(conn->conn_);
	 }
 }

 bool DBIOMysql::beginTransaction(ConnectionWrapBasePtr connPtr)
 {
	 if (connPtr)
	 {
		 ConnectionWrapMysqlPtr conn = boost::static_pointer_cast<ConnectionWrapMysql>(connPtr);
		 return pool_->beginTransaction(conn->conn_);
	 }

	 return false;
 }

 bool DBIOMysql::commitTransaction(ConnectionWrapBasePtr connPtr)
 {
	 if (connPtr)
	 {
		 ConnectionWrapMysqlPtr conn = boost::static_pointer_cast<ConnectionWrapMysql>(connPtr);
		 return pool_->commitTransaction(conn->conn_);
	 }
	 return false;
 }

 bool DBIOMysql::rollBack(ConnectionWrapBasePtr connPtr)
 {
	 if (connPtr)
	 {
		 ConnectionWrapMysqlPtr conn = boost::static_pointer_cast<ConnectionWrapMysql>(connPtr);
		 return pool_->rollBack(conn->conn_);
	 }
	 return false;
 }

 void  DBIOMysql::close()
 {
	if (pool_)
	{
		pool_->close();
		pool_ = NULL;
	}

	stopCheckConnection();
 }

 void DBIOMysql::checkDbConnections()
 {
	if (pool_)
	{
		pool_->checkDbConnections();
	}
 }
