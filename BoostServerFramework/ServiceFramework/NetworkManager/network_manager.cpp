#include "network_manager.h"
NetworkManager* NetworkManager::instance_ = nullptr;
void NetworkManager::tcpAcceptor(const unsigned short& port)
{
	_acceptor.reset(new TcpAcceptor(IOManager::instance().ioService(), port));
}


TcpConnectionPtr NetworkManager::connection(const std::string& userID,
    const std::string& sessionID,
    const std::string& connectionType) const
{
    return SessionManager::instance().connection(userID, sessionID, connectionType);
}


void NetworkManager::addConnection(const std::string& userID,
    const std::string& sessionID,
    const std::string& connectionType,
    const TcpConnectionPtr conn) const
{
    return SessionManager::instance().addConnection(userID, sessionID, connectionType, conn);
}


void NetworkManager::removeConnection(const std::string& userID,
    const std::string& sessionID,
    const std::string& connectionType,
    const std::string& connectionID) const
{
    return SessionManager::instance().removeConnection(userID, sessionID, connectionType, connectionID);
}


void NetworkManager::modifyConnection(const std::string& userID,
    const std::string& sessionID,
    const std::string& connectionType,
    const TcpConnectionPtr conn) const
{
    return SessionManager::instance().modifyConnection(userID, sessionID, connectionType, conn);
}