#pragma once
#include <map>
#include <boost/shared_ptr.hpp>
#include "connections.h"


typedef std::map<std::string, ConnectionsPtr> ConnectionsPtrMap;

/**
 * @brief session集合类
 */
class Sessions
{
public:
    const ConnectionsPtrMap& connectionsPtrMap() const;

    ConnectionsPtr session(const std::string sessionType);
    void addSession(const std::string sessionType,
            ConnectionsPtr connections);
    void removeSession(const std::string sessionType);
    void modifySession(const std::string sessionType,
            ConnectionsPtr connections);

    std::size_t sessionSize() const;

private:
    ConnectionsPtrMap _connectionsPtrMap;   /**< 存放连接集合的容器 */

};
typedef boost::shared_ptr<Sessions> SessionsPtr;
