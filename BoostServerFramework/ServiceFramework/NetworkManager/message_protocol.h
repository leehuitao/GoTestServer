#ifndef MsgProtocol_H
#define MsgProtocol_H

#include <string>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

#define MSG_PROTOCOL_VER   			(1)
#define MSG_PROTOCOL_HEAD_SIZE		(16)

enum MsgProtocolEncryptFuc
{
	kFuncNone,
	kFuncAes,
	kFuncSM4
};

typedef std::vector<std::string> ParamsVector;
typedef std::map<std::string, std::string> PropMap;
typedef std::vector<std::string> PropKeyVector;
typedef std::vector<std::string> PropValueVector;

class TcpConnection;
class UdpDispatch;

/** 请求状态枚举 */
enum RequestState {
    kRequestStateNone = 0,   /**< 默认状态 */
    kRequestStateParam,      /**< 解析参数状态 */
    kRequestStateProp,       /**< 解析属性状态 */
    kRequestStateBody,       /**< 解析体状态 */
    kRequestStateDone,       /**< 解析完成状态 */
    kRequestStateError       /**< 解析错误 */
};

/** 缓冲区大小 */
enum BufferInfo {
    kBufferInfoSize = 4096    /**< 默认大小 */
};

/** 消息头 **/
struct MessageHeader
{
	unsigned int    packet_length_;
	unsigned int    order_num_;
	unsigned char   header_length_;
	unsigned char   protocol_ver_;
	unsigned short  method_;
	unsigned char	status_num_;
	unsigned char	encript_func_;
	unsigned char	extend1_;
	unsigned char	extend2_;
};

/** 缓冲区 */
struct MessageBuffer
{
    char _data[kBufferInfoSize + 1];  /**< 缓冲区串 */
    boost::asio::ip::udp::endpoint _endpoint;
};
typedef boost::shared_ptr<MessageBuffer> MessageBufferPtr;

/** 请求缓冲区 */
struct RequestBuffer
{
	RequestBuffer(RequestBuffer& in) {
		*this = in;
		//AntUtils::mmonitor::instance().increase("RequestBuffer");
	}
	RequestBuffer()
	{
		method_ = 0;
		order_num_ = 0;
		status_num_ = 0;
		encript_func_ = 0;
		req_status_ = kRequestStateNone;
		//AntUtils::mmonitor::instance().increase("RequestBuffer");
	}
	~RequestBuffer()
	{
		//AntUtils::mmonitor::instance().reduce("RequestBuffer");
	}

	void printObject() {

		std::cout << "[R]:" "  _METHOD: 0x" << std::hex << method_ << std::endl;
		std::cout  << "OrderNum:" << std::dec << order_num_ << std::endl;
		std::cout << "encript_func_:" << encript_func_ << std::endl;

		for (ParamsVector::iterator iter = params_.begin(); iter != params_.end(); iter++) {
			std::cout << *iter << "  ";
		}
		std::cout << std::endl;

		for (PropMap::iterator iter = props_.begin(); iter != props_.end(); iter++) {
			std::cout << iter->first << ":" << iter->second << std::endl;
		}
		std::cout << body_ << std::endl;
		std::cout << "-----+----------+-----------+-----------+------" << std::endl;
	}

	unsigned short method_;                    /**< method数据 */
	unsigned int  order_num_;
	unsigned char  status_num_;
	unsigned char encript_func_;
    ParamsVector params_;                   /**< params数据 */
    PropMap props_;                         /**< props数据 */
    std::string body_;                      /**< body数据 */

    std::size_t req_status_;                 /**< 请求状态 */
//    PropKeyVector en_prop_keys_;              /**< 加密版prop的键 */
//    PropValueVector en_prop_values_;          /**< 加密版prop的值 */

    boost::shared_ptr<TcpConnection> conn_; /**< 当前连接的指针 */
    boost::shared_ptr<UdpDispatch> dispatch_;
    boost::asio::ip::udp::endpoint endpoint_;
};
typedef boost::shared_ptr<RequestBuffer> RequestBufferPtr;
typedef boost::function<void(RequestBufferPtr)> RequestHandler;

/** 应答缓冲区 */
struct ResponseBuffer
{
	ResponseBuffer(ResponseBuffer& in) 
	{
		*this = in;
		//AntUtils::mmonitor::instance().increase("ResponseBuffer");
	}
	ResponseBuffer()
	{
		method_ = 0;
		order_num_ = 0;
		status_num_ = 0;
		encript_func_ = 0;
		time_send_ = std::time(0);
		//AntUtils::mmonitor::instance().increase("ResponseBuffer");
	}
	~ResponseBuffer()
	{
		//AntUtils::mmonitor::instance().reduce("ResponseBuffer");
	}

	unsigned short method_;                    /**< method数据 */
	unsigned int  order_num_;
	unsigned char  status_num_;
	unsigned char encript_func_;
    ParamsVector params_;                   /**< params数据 */
    PropMap props_;                         /**< props数据 */
    std::string body_;                      /**< body数据 */

    boost::shared_ptr<TcpConnection> conn_;
    boost::shared_ptr<UdpDispatch> dispatch_;
    boost::asio::ip::udp::endpoint endpoint_;
	std::time_t 	  time_send_;
};
typedef boost::shared_ptr<ResponseBuffer> ResponseBufferPtr;

struct EdgeRequest
{
	ResponseBufferPtr request_;
	RequestBufferPtr  org_request_;
	unsigned int 	  org_order_num;
	std::time_t 	  time_send_;
};
typedef boost::shared_ptr<EdgeRequest> EdgeRequestPtr;

const unsigned short kUnknownCmder(0x0000);


#if defined(WIN32)
#define htobe32 htonl
#define htobe16 htons
#define be32toh ntohl
#define be16toh ntohs
#endif


#endif // MsgProtocol_H
