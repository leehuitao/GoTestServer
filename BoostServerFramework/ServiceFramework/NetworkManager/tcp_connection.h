#pragma once

#include <queue>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/strand.hpp>
#include "message_protocol.h"
#include <boost/format.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/io_service.hpp>
#include <memory>
#include <boost/thread.hpp>
#include "parse_request_data.h"
#include "bundle_response_data.h"
#include "msghandler_repository.h"
class MessageBuffer;
class BundleResponseData;

/**
 * @brief tcp基础连接类
 */
class TcpConnection: public boost::enable_shared_from_this<TcpConnection>
{
    typedef boost::function<void(const std::string&, const std::string&, const std::string&, const std::string&)> fException;
    typedef boost::function<void(const std::size_t&, boost::shared_ptr<TcpConnection>)> fRecvCallback;
    typedef boost::function<void(const std::size_t&, boost::shared_ptr<TcpConnection>)> fSendCallback;
public:
    typedef boost::asio::ip::tcp tcp;

    explicit TcpConnection(boost::asio::io_service& service,
    		const std::string& id);
    virtual ~TcpConnection();

    void setConnectionProps(const std::string&,const std::string& ,const std::string& ,const std::string& server_name = "");
	void setAsynExcuteEventMod(bool asyn_excute_event);

    const std::string& userID() const;
    const std::string& sessionType() const;
    const std::string& connectionType() const;
    const std::size_t& outStandingBufferSize() const;
    const std::string& ServerName() const;

    tcp::socket& socket();
    boost::asio::io_service::strand& strand();
    void close();
    bool isIpv4() const;
    bool isIpv6() const;
    bool isMulticast() const;
    std::string ip() const;
    unsigned short port() const;
    const std::string& id() const;

    void startReceive();

    void sendBundleMessage(const ResponseBufferPtr data);

    void handleTimeout();

    bool IsOverTime();

    time_t login_request_time;

    void setExceptionCallBack(fException func = fException());
    void setRecvAfterCallBack(fRecvCallback operation = fRecvCallback());
    void setSendAfterCallBack(fSendCallback operation = fSendCallback());

protected:
    void handleSend(boost::shared_ptr<std::string>, const boost::system::error_code&,const std::size_t&);
    void handleReceive(const boost::system::error_code&,const std::size_t&,boost::shared_ptr<MessageBuffer>);
    void handleData(MessageBufferPtr, const std::size_t&);
    std::size_t sendMessage(std::string msg);
    void sendResponse(ResponseBufferPtr data);
private:
	

    std::size_t _outStandingBufferSize;
    int  _headSize;
    bool _asyn_excute_event_mod;
	std::string	_ipaddress;
    std::string _userID;                            /**< 用户的唯一标示 */
    std::string _sessionType;                       /**< session类型 */
    std::string _connectionType;                    /**< 连接类型 */
    std::string server_name_;
    std::string _id;                                /**< 连接ID */
    tcp::socket _sock;                              /**< 连接里的sock */
    std::time_t _lastRecvTime;                      /**< 最后一次接受时间 */
    boost::asio::io_service& _netIOService;         /**< 通讯所需io */
    boost::asio::deadline_timer _netTimer;          /**< 连接定时检查器 */
    boost::asio::io_service::strand  _strand;
    std::vector<char> _inputBuffer;

	std::queue<ResponseBufferPtr> response_queue_;
    fException      _operationException;
    fRecvCallback   _operationRecvAfter;
    fSendCallback   _operationSendAfter;
};
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

