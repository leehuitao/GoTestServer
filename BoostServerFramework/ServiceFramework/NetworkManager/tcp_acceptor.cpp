

#include <iostream>
#include <boost/bind.hpp>
#include "tcp_connection.h"
#include "tcp_acceptor.h"
#include "io_manager.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <comdef.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <codecvt>
#include <Winldap.h>
#define EFFECTIVENUM 30


/**
 * @brief 初始化TcpAcceptor对象
 * @param service 运行所需io
 * @param listenPort 监听端口
 * @param operation 回调函数
 */
TcpAcceptor::TcpAcceptor(boost::asio::io_service& service,unsigned short listenPort):
                _svc(service),
                _acceptor(_svc, tcp::endpoint(tcp::v4(), listenPort)),
                clearTimer(IOManager::instance().ioService())
{
    start();
}
/**
 * @brief 处理监听成功
 * @param conn 监听到的连接
 * @param error 错误代码
 */
void TcpAcceptor::handleAccept(TcpConnectionPtr conn,
        const boost::system::error_code& error)
{
    std::cout << "new connection ";
    if (!error) {
        if (!_operationAfterAccept.empty())
            _operationAfterAccept(conn);

        conn->startReceive();//后续需要登录成功后将链接放到sessionManager
        conn->handleTimeout();
    }
    start();
}

std::string wstrToUtf8(const std::wstring& src)
{
	std::string ret;
	try
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
		ret = convert.to_bytes(src);
	}
	catch (...)
	{
	}
	return ret;
}
/**
* @brief 获取包含花括号的串值
* @return 返回包含花括号的串值
*/
const std::string randUuidToStringWithBraces()
{
	GUID			guid;
	HRESULT			hr = S_OK;
	OLECHAR			szGuid[50];
	std::wstring	strWGUID;
	std::string		strGUID;

	hr = CoCreateGuid(&guid);
	if (hr != S_OK)
		return std::string();

	StringFromGUID2(guid, szGuid, 50);
	strWGUID = szGuid;
	strGUID = wstrToUtf8(strWGUID);

	return strGUID;
}
inline void TcpAcceptor::start()
{
    std::string id = randUuidToStringWithBraces();
    TcpConnectionPtr conn(new TcpConnection(IOManager::instance().ioService(), id ));

	_acceptor.async_accept(conn->socket(),
			boost::bind(&TcpAcceptor::handleAccept, this, conn,
					boost::asio::placeholders::error));
}
