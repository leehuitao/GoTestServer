
#include "connection_pool_mysql.hpp"

void MySqlConnectionPool::setParam(const std::string& serverAddr,
		const std::size_t& port,
		const std::string& username,
		const std::string& passwd,
		const std::string& dbname,
		const std::size_t& maxSize)
{
	std::cout<<"MySqlConnectionPool::setParam "<<
		" serverAddr="<< serverAddr <<
		" port="<< port <<
		" passwd="<< passwd<<
		" dbname="<< dbname<<
		" maxSize="<< maxSize<<std::endl;
	_serverAddr = serverAddr;
	_port = port;
	_username = username;
	_passwd = passwd;
	_dbname = dbname;
	if (maxSize)
		poolMaxSize_ = maxSize;

}

bool MySqlConnectionPool::initConnection()
{
	connContainerNoShare_.set_capacity(poolMaxSize_);
    boost::system::error_code ec;
	return true;
}

void MySqlConnectionPool::close()
{
	 boost::mutex::scoped_lock lock(poolMutex_);
	 for (int i = 0; i < connContainerNoShare_.size(); ++i)
	 {
		Connection* connPtr = connContainerNoShare_[i];

		if (connPtr)
			::mysql_close(connPtr);
     }

	 connContainerNoShare_.clear();
}

MySqlConnectionPool::Connection* MySqlConnectionPool::mySqlconnection()
{
    Connection* connPtr = NULL;

    std::time_t begin;
    std::time(&begin);

    while (1) {
        boost::mutex::scoped_lock lock(poolMutex_);
        if (connContainerNoShare_.size() == 0) {
			if (poolSize_ <= poolMaxSize_)
			{
				this->mySqlCreateConnection();
				if (connContainerNoShare_.size() == 0)
				{
					return NULL;
				}
			}
            continue;
        }

        connPtr = connContainerNoShare_.front();
		connContainerNoShare_.pop_front();
        if (connPtr) {
            break;

        } else {
            this->mySqlCreateConnection();
            continue;;
        }
    }

    return connPtr;
}

MySqlConnectionPool::Connection* MySqlConnectionPool::mySqlCreateConnection()
{
	Connection* connPtr;
	connPtr = ::mysql_init(NULL);

    if (!::mysql_real_connect(&*connPtr, _serverAddr.c_str(), _username.c_str(), _passwd.c_str(),
            _dbname.c_str(), _port, NULL, 0)) {
		
        ::mysql_close(&*connPtr);
		connPtr = NULL;
        return NULL;
    }

    if (::mysql_set_character_set(&*connPtr, "utf8")) {
		
        ::mysql_close(&*connPtr);
        return NULL;
    }

    std::string query("SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED;");
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		
        ::mysql_close(&*connPtr);
        return NULL;
    }
    query.clear();
    query = "USE "; query += _dbname; query += ";";
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		
        ::mysql_close(&*connPtr);
        return NULL;
    }
    query.clear();
    query = "SET GLOBAL init_connect='SET AUTOCOMMIT=1';";
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		
        ::mysql_close(&*connPtr);
        return NULL;
    }

    if (connPtr) {
		connContainerNoShare_.push_back(connPtr);

    }
	poolSize_++;
    return connPtr;

}


void MySqlConnectionPool::releaseConnection(Connection* ptrConn)
{
    boost::mutex::scoped_lock lock(poolMutex_);
    if (ptrConn) {
		connContainerNoShare_.push_back(ptrConn);
    }
}

void MySqlConnectionPool::checkDbConnections()
{
	 boost::mutex::scoped_lock lock(poolMutex_);
	 int index = 0;
	 for (ConnectionCirularNoShare::iterator iter = connContainerNoShare_.begin();iter != connContainerNoShare_.end();)
	 {
		Connection* connPtr = *iter;
		if (::mysql_ping(&*connPtr) != 0)
		{
		    ::mysql_close(&*connPtr);
		    iter = connContainerNoShare_.erase(iter);
		    index++;
			poolSize_--;
		}
		else
		{
			iter++;
		}
     }
}

bool MySqlConnectionPool::beginTransaction(Connection* connPtr)
{
    if (!connPtr) {
		
        return false;
    }
    std::string query("SET AUTOCOMMIT=0;");
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		
        return false;
    }
    query.clear(); query = "START TRANSACTION;";
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		  return false;
    }

    return true;
}

bool MySqlConnectionPool::commitTransaction(Connection* connPtr)
{
    if (!connPtr) {
		 return false;
    }
    std::string query("COMMIT;");
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		 this->releaseConnection(connPtr);
        return false;
    }
    query.clear(); query = "SET AUTOCOMMIT=1;";
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		  this->releaseConnection(connPtr);
        return false;
    }

    this->releaseConnection(connPtr);
    return true;
}

bool MySqlConnectionPool::rollBack(Connection* connPtr)
{
    if (!connPtr) {
		 return false;
    }
    std::string query("ROLLBACK;");
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		 this->releaseConnection(connPtr);
        return false;
    }
    query.clear(); query = "SET AUTOCOMMIT=1;";
    if (::mysql_real_query(&*connPtr, query.c_str(), (unsigned int)strlen(query.c_str()))) {
		 this->releaseConnection(connPtr);
        return false;
    }
    this->releaseConnection(connPtr);
    return true;
}


