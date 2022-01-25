
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "session_manager.h"


/**
 * @brief 创建SessionManager的单例对象
 * @return 返回SessionManager对象
 */
SessionManager& SessionManager::instance()
{
    static SessionManager _gInstance;
    return _gInstance;
}

/**
 * @brief 获取一个session
 * @param userID 用户的唯一标示
 * @param sessionType session类型
 * @return session的指针
 */
ConnectionsPtr SessionManager::session(const std::string& userID,
        const std::string& sessionType)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);
    if (it != _sessionsPtrMap.end()) {
        return it->second->session(sessionType);
    } else {
        return ConnectionsPtr();
    }
}

/**
 * @brief 添加一个sessionn
 * @param userID 用户的唯一标示
 * @param sessionType session类型
 * @param connections 连接集合
 */
void SessionManager::addSession(const std::string& userID,
        const std::string& sessionType,
        const ConnectionsPtr connections)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);
    if (it != _sessionsPtrMap.end()) {
        it->second->removeSession(sessionType);
        it->second->addSession(sessionType, connections);
    } else {
        it->second->addSession(sessionType, connections);
    }
}

/**
 * @brief 删除一个session
 * @param userID 用户的唯一标示
 * @param sessionType session类型
 */
void SessionManager::removeSession(const std::string& userID,
        const std::string& sessionType)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);
    if (it != _sessionsPtrMap.end()) {
        it->second->removeSession(sessionType);
    }
}

/**
 * @brief 更改一个session
 * @param userID 用户的唯一标示
 * @param sessionType session类型
 * @param connections 连接集合
 */
void SessionManager::modifySession(const std::string& userID,
        const std::string& sessionType,
        const ConnectionsPtr connections)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);
    if (it != _sessionsPtrMap.end()) {
        it->second->modifySession(sessionType, connections);
    }
}

/**
 * @brief 获取一个连接
 * @param userID 用户的唯一标示
 * @param sessionType session类型
 * @param connectionType 连接类型
 * @return 返回连接的指针
 */
TcpConnectionPtr SessionManager::connection(const std::string& userID,
        const std::string& sessionType,
        const std::string& connectionType)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);
    if (it != _sessionsPtrMap.end()) {
        if (it->second->session(sessionType)) {
            return it->second->session(sessionType)->connection(connectionType);
        } else {
            return TcpConnectionPtr();
        }
    } else {
        return TcpConnectionPtr();
    }
}

/**
 * @brief 添加一个连接
 * @param userID 用户的唯一标示
 * @param sessionType session类型
 * @param connectionType 连接类型
 * @param conn 指定连接
 */
void SessionManager::addConnection(const std::string& userID,
        const std::string& sessionType,
        const std::string& connectionType,
        const TcpConnectionPtr conn)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    ConnectionsPtr connectionsPtr(new Connections);

    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);
    if (it != _sessionsPtrMap.end()) {
        if (it->second->session(sessionType))
        {
            if (it->second->session(sessionType)->connection(connectionType))
            {
                it->second->session(sessionType)->modifyConnection(connectionType, conn);
                conn->setConnectionProps(userID, sessionType, connectionType);
                conn->setExceptionCallBack(boost::bind(&SessionManager::removeConnection,
                        this, _1, _2, _3, _4, true));

            } else {
                it->second->session(sessionType)->addConnection(connectionType, conn);
                conn->setConnectionProps(userID, sessionType, connectionType);
                conn->setExceptionCallBack(boost::bind(&SessionManager::removeConnection,
                        this, _1, _2, _3, _4, true));
            }

        } else {
            connectionsPtr->addConnection(connectionType, conn);
            conn->setConnectionProps(userID, sessionType, connectionType);
            conn->setExceptionCallBack(boost::bind(&SessionManager::removeConnection,
                    this, _1, _2, _3, _4, true));
            it->second->addSession(sessionType, connectionsPtr);
        }

    } else {
        SessionsPtr sessionsPtr(new Sessions);
        connectionsPtr->addConnection(connectionType, conn);
        conn->setConnectionProps(userID, sessionType, connectionType);
        conn->setExceptionCallBack(boost::bind(&SessionManager::removeConnection,
                this, _1, _2, _3, _4, true));
        sessionsPtr->addSession(sessionType, connectionsPtr);
        _sessionsPtrMap.insert(std::make_pair(userID, sessionsPtr));
    }
}

/**
 * @brief 删除一个连接
 * @param userID 用户的唯一标示
 * @param sessionType session类型
 * @param connectionType 连接类型
 */
void SessionManager::removeConnection(const std::string& userID,
        const std::string& sessionType,
        const std::string& connectionType,
        const std::string& connectionId,
		bool exception_call)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);

    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);
    if (it != _sessionsPtrMap.end()) {
        if (it->second->session(sessionType)) {
            if (it->second->session(sessionType)->connection(connectionType)) {
                if (it->second->session(sessionType)->connection(connectionType)->id()
                        != connectionId)
                    return ;

                it->second->session(sessionType)->removeConnection(connectionType);

                if (!it->second->session(sessionType)->tcpConnectionPtrMap().size())
                    it->second->removeSession(sessionType);

                if (!it->second->sessionSize())
                    _sessionsPtrMap.erase(it);

                if (exception_call && !_operationException.empty())
                	_operationException(userID, sessionType, connectionType, connectionId);
            }
        }
    }
}

/**
 * @brief 更改一个连接
 * @param userID 用户的唯一标示
 * @param sessionType session类型
 * @param connectionType 连接类型
 * @param conn 指定连接
 */
void SessionManager::modifyConnection(const std::string& userID,
        const std::string& sessionType,
        const std::string& connectionType,
        const TcpConnectionPtr conn)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);
    if (it != _sessionsPtrMap.end()) {
        if (it->second->session(sessionType)) {
            if (it->second->session(sessionType)->connection(connectionType)) {
                it->second->session(sessionType)->modifyConnection(connectionType, conn);
                conn->setConnectionProps(userID, sessionType, connectionType);
            }
        }
    }
}

std::vector<TcpConnectionPtr> SessionManager::userConnections(const std::string& userID)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    std::vector<TcpConnectionPtr> connections;

    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);

    if (it != _sessionsPtrMap.end()) {
        ConnectionsPtrMap connectionsPtrMap = it->second->connectionsPtrMap();
        ConnectionsPtrMap::iterator iter = connectionsPtrMap.begin();

        for (; iter != connectionsPtrMap.end(); ++iter) {
            TcpConnectionPtrMap tcpConnectionPtrMap = iter->second->tcpConnectionPtrMap();
            TcpConnectionPtrMap::iterator iterer = tcpConnectionPtrMap.begin();

            for (; iterer != tcpConnectionPtrMap.end(); ++iterer) {
                connections.push_back(iterer->second);
            }
        }
    }

    return connections;
}

void SessionManager::userConnectionsSendResponseBuffer(const std::string& userID,
        ResponseBufferPtr data)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    std::vector<TcpConnectionPtr> connections;

    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);

    if (it != _sessionsPtrMap.end()) {
        ConnectionsPtrMap connectionsPtrMap = it->second->connectionsPtrMap();
        ConnectionsPtrMap::iterator iter = connectionsPtrMap.begin();

        for (; iter != connectionsPtrMap.end(); ++iter) {
            TcpConnectionPtrMap tcpConnectionPtrMap = iter->second->tcpConnectionPtrMap();
            TcpConnectionPtrMap::iterator iterer = tcpConnectionPtrMap.begin();

            for (; iterer != tcpConnectionPtrMap.end(); ++iterer) {
                iterer->second->sendBundleMessage(data);
            }
        }
    }
}

void SessionManager::setExceptionCallBack(boost::function<void(const std::string&,
        const std::string&,
        const std::string&,
		const std::string&)> operation)
{
	_operationException = operation;
}

/**
 * @brief 获取连接数
 * @return 返回连接数
 */
std::string SessionManager::connectionsNumber()
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    std::size_t connectionNumber = 0;

    SessionsPtrMap::iterator it = _sessionsPtrMap.begin();

    for (; it != _sessionsPtrMap.end(); ++it) {
        ConnectionsPtrMap connectionsPtrMap = it->second->connectionsPtrMap();
        ConnectionsPtrMap::iterator iter = connectionsPtrMap.begin();

        for (; iter != connectionsPtrMap.end(); ++iter) {
            connectionNumber += iter->second->tcpConnectionPtrMap().size();
        }
    }

    return boost::lexical_cast<std::string>(connectionNumber);
}

/**
 * @brief 获取单个用户的连接数
 * @param userID 用户的唯一标示
 * @return 返回单个用户的连接数
 */
std::string SessionManager::userConnectionNumber(const std::string& userID)
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    std::size_t connectionNumber = 0;

    SessionsPtrMap::iterator it = _sessionsPtrMap.find(userID);

    if (it != _sessionsPtrMap.end()) {
        ConnectionsPtrMap connectionsPtrMap = it->second->connectionsPtrMap();
        ConnectionsPtrMap::iterator iter = connectionsPtrMap.begin();

        for (; iter != connectionsPtrMap.end(); ++iter) {
            connectionNumber += iter->second->tcpConnectionPtrMap().size();
        }
    }

    return boost::lexical_cast<std::string>(connectionNumber);
}

std::string SessionManager::onlineUsersNumber()
{
    boost::recursive_mutex::scoped_lock lock(_mutex);
    std::size_t onlineUserNumber = 0;

    SessionsPtrMap::iterator it = _sessionsPtrMap.begin();
    for (; it != _sessionsPtrMap.end(); ++it) {
        ++onlineUserNumber;
    }

    return boost::lexical_cast<std::string>(onlineUserNumber);
}

std::vector<std::string> SessionManager::onlineUsers()
{
    boost::recursive_mutex::scoped_lock lock(_mutex);

    std::vector<std::string> users;

    SessionsPtrMap::iterator it = _sessionsPtrMap.begin();
    for (; it != _sessionsPtrMap.end(); ++it) {
        users.push_back(it->first);
    }

    return users;
}
