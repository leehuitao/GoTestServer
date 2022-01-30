#pragma once
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

template <class Conn>
class ConnectionPoolAbstract
{
public:
    ConnectionPoolAbstract(): poolMaxSize_(1), poolSize_(0){}

    typedef Conn Connection;
    typedef boost::shared_ptr<Conn> ConnectionPtr;

    virtual ConnectionPtr connection() = 0;
    virtual bool initConnection() = 0;
    virtual ConnectionPtr createConnection() = 0;
    virtual void releaseConnection(ConnectionPtr ptrConn) = 0;
    virtual void checkDbConnections() = 0;
    virtual void close() = 0;

    virtual bool beginTransaction(ConnectionPtr connPtr) = 0;
    virtual bool commitTransaction(ConnectionPtr connPtr) = 0;
    virtual bool rollBack(ConnectionPtr connPtr) = 0;

protected:
    typedef boost::circular_buffer_space_optimized<ConnectionPtr> ConnectionCirular;
	typedef boost::circular_buffer_space_optimized<Connection*> ConnectionCirularNoShare;

    ConnectionCirular connContainer_;           /* 存储数据库连接的容器 */
	ConnectionCirularNoShare connContainerNoShare_;
    std::size_t poolMaxSize_;                   /*  连接池的大小 */
	std::size_t poolSize_;
    boost::mutex poolMutex_;                        /* 多线程所需锁 */
};

#pragma once