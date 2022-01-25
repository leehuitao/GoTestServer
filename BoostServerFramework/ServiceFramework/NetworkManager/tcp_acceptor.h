
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/thread/mutex.hpp>
#include "message_protocol.h"
#include <unordered_map>

class TcpConnection;

/**
 * @brief tcp监听类
 */
class TcpAcceptor
{
public:
    typedef boost::asio::ip::tcp tcp;

    explicit TcpAcceptor(boost::asio::io_service& service,
            unsigned short listenPort);
private:
    void handleAccept(boost::shared_ptr<TcpConnection> conn,
            const boost::system::error_code& error);
    void start();
	
private:
    
    boost::asio::io_service& _svc;          /**< 监听需要使用的io */
    tcp::acceptor _acceptor;                /**< 监听对象 */
    bool _asyn_excute_event_mod;
    boost::function<void(boost::shared_ptr<TcpConnection> conn)> _operationAfterAccept;
    boost::mutex connectionMutex_;
    boost::asio::deadline_timer clearTimer;
    std::vector<std::string> errorVector;
    bool enbaleCheck;
    unsigned short loginCount;
    std::unordered_map<std::string, unsigned short> clientLoginNumber_;
};
