#pragma once
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"
#include <queue>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>
#include <boost/logic/tribool.hpp>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include "tcp_connection.h" 
#include "tcp_acceptor.h"
#include "io_manager.h"
#include "session_manager.h"
#include "io_manager.h"
typedef boost::unique_lock<boost::mutex> Lock;

class NetworkManager : private boost::noncopyable
{
public:
    static NetworkManager* Instance() {
        static boost::mutex mutex;
        if (!instance_) {
            Lock locker(mutex);
            if (!instance_) {
                instance_ = new NetworkManager();
            }
        }
        return instance_;
    }
    /** 监听tcp */
    void tcpAcceptor(const unsigned short& port);
    /** 获取一条连接 */
    TcpConnectionPtr connection(const std::string& userID,
        const std::string& sessionID,
        const std::string& connectionType) const;

    /** 一条连接 */
    void addConnection(const std::string& userID,
        const std::string& sessionID,
        const std::string& connectionType,
        const TcpConnectionPtr conn) const;

    /** 删除一条连接 */
    void removeConnection(const std::string& userID,
        const std::string& sessionID,
        const std::string& connectionType,
        const std::string& connectionID) const;

    /** 更改一条连接 */
    void modifyConnection(const std::string& userID,
        const std::string& sessionID,
        const std::string& connectionType,
        const TcpConnectionPtr conn) const;
private:
    static NetworkManager* instance_;
    boost::shared_ptr<TcpAcceptor> _acceptor;    /**< 监听依赖所需对象 */
    boost::shared_ptr<UdpDispatch> _dispatch;
    int	 _connect_count;
};

