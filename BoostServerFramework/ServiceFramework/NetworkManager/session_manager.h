
#ifndef _INC_BIGANTSERVER_ANTNETIO_SESSION_MANAGER_H
#define _INC_BIGANTSERVER_ANTNETIO_SESSION_MANAGER_H

#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "sessions.h"
#include "connections.h"

/**
 * @brief session管理类
 */
class SessionManager: private boost::noncopyable
{
public:
    static SessionManager& instance();

    ConnectionsPtr session(const std::string& userID,
            const std::string& sessionType);
    void addSession(const std::string& userID,
            const std::string& sessionType,
            const ConnectionsPtr connections);
    void removeSession(const std::string& userID,
            const std::string& sessionType);
    void modifySession(const std::string& userID,
            const std::string& sessionType,
            const ConnectionsPtr connections);

    TcpConnectionPtr connection(const std::string& userID,
            const std::string& sessionType,
            const std::string& connectionType);
    void addConnection(const std::string& userID,
            const std::string& sessionType,
            const std::string& connectionType,
            const TcpConnectionPtr conn);
    void removeConnection(const std::string& userID,
            const std::string& sessionType,
            const std::string& connectionType,
            const std::string& connectionId,
			bool exception_call = false);
    void modifyConnection(const std::string& userID,
            const std::string& sessionType,
            const std::string& connectionType,
            const TcpConnectionPtr conn);

    std::vector<TcpConnectionPtr> userConnections(const std::string& userID);
    void userConnectionsSendResponseBuffer(const std::string& userID,
            ResponseBufferPtr data);

    void setExceptionCallBack(boost::function<void(const std::string&,
            const std::string&,
            const std::string&,
            const std::string&)> operation=
                    boost::function<void(const std::string&,
                            const std::string&,
                            const std::string&,
                            const std::string&)>());

    std::string connectionsNumber();
    std::string userConnectionNumber(const std::string& userID);
    std::string onlineUsersNumber();
    std::vector<std::string> onlineUsers();

private:
    typedef std::map<std::string, SessionsPtr> SessionsPtrMap;

    SessionsPtrMap _sessionsPtrMap;     /**< 存放session的容器 */
    boost::recursive_mutex _mutex;      /**< session管理所需的锁 */

    boost::function<void(const std::string& userID,
            const std::string& sessionID,
            const std::string& connectionType,
			const std::string& connectionID)> _operationException;

};


#endif // _INC_BIGANTSERVER_ANTNETIO_SESSION_MANAGER_H
