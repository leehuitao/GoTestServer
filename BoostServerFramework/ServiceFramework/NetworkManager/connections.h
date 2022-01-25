#pragma once
#include <map>
#include <boost/enable_shared_from_this.hpp>
#include "tcp_connection.h"


typedef std::map<std::string, TcpConnectionPtr> TcpConnectionPtrMap;

/**
 * @brief 连接集合类
 */
class Connections: public boost::enable_shared_from_this<Connections>
{
public:
    const TcpConnectionPtrMap& tcpConnectionPtrMap() const;

    TcpConnectionPtr connection(const std::string connectionType);
    void addConnection(const std::string connectionType, TcpConnectionPtr conn);
    void removeConnection(const std::string connectionType);
    void removeCheckConnection(TcpConnectionPtr connection);
    void modifyConnection(const std::string connectionType, TcpConnectionPtr conn);

private:
    typedef boost::asio::ip::tcp tcp;

    TcpConnectionPtrMap _tcpConnectionPtrMap;   /**< 存放连接的容器 */

};
typedef boost::shared_ptr<Connections> ConnectionsPtr;
