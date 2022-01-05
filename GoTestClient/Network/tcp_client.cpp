#include "tcp_client.h"

TcpClient::TcpClient(QObject *parent)
{
    init();
}

void TcpClient::sendLogin(QString ip, int port, LoginBody body)
{
    m_serverip = ip;
    m_serverport = port;
    m_loginBody = body;
    if(m_socket != nullptr){
        disconnect(m_socket,&QTcpSocket::connected,this,&TcpClient::connected);
        disconnect(m_socket,&QTcpSocket::readyRead,this,&TcpClient::receiveData);
        m_socket->close();
        m_socket = nullptr;
    }
    m_socket = new QTcpSocket;
    m_socket->connectToHost(QHostAddress(ip),port);
    connect(m_socket,&QTcpSocket::connected,this,&TcpClient::connected);
    connect(m_socket,&QTcpSocket::readyRead,this,&TcpClient::receiveData);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TcpClient::slotDisconnect);
}

void TcpClient::sendLogout(LoginBody body)
{
    Pack pack(body,Logout,0);
    auto data = pack.toByte();
    m_socket->write(data);
}

void TcpClient::sendMsg(MsgBody body, int method, int methodType)
{
    Pack pack(body,method,methodType);
    auto data = pack.toByte();
    m_socket->write(data);
}

void TcpClient::sendFile(FileBody body, int method, int methodType)
{

}

void TcpClient::init()
{

}

void TcpClient::connected()
{
    qDebug()<<"tcp connected";
    Pack pack(m_loginBody,Login,0);
    auto data = pack.toByte();
    m_socket->write(data);
}

void TcpClient::slotDisconnect(QAbstractSocket::SocketError socketError)
{
    signLoginStatus(0,"连接断开");
}

void TcpClient::receiveData()
{
    //缓冲区没有数据，直接无视
    if( m_socket->bytesAvailable() <= 0 )
    {
        return;
    }

    QByteArray buffer;
    buffer = m_socket->readAll();

    m_buffer.append(buffer);
    int size= -1;
    int method = -1;
    int methodType = -1;
    int totalLen = m_buffer.size();
    while( totalLen )
    {
        QDataStream packet(m_buffer);

        if( totalLen < HeaderSize )//包头长度
        {
            break;
        }
        packet >> size >> method >> methodType;

        if( totalLen < size )
        {
            break;
        }
        char *rbytes = new char[size - HeaderSize];
        memset(rbytes,0,size-HeaderSize);
        memcpy(rbytes,m_buffer.data()+HeaderSize,size-HeaderSize);
        QByteArray arr(rbytes);
        arr.resize(size - HeaderSize);
        if(method == Login){
            LoginBody body;
            body = m_packProcess.parseLoginPack(arr);
            signLoginStatus(body.LoginStatus,body.LoginStatus ? "登录成功":"登录失败");

        }else if(method == Logout){
            signLoginStatus(0," user logout");
        }else if(method == Msg){
            MsgBody body;
            body = m_packProcess.parseMsgPack(arr);
            signRecvMsg(body);
        }else if(method == SendFileData){
            qDebug()<<"SendFileData";
        }else if(method == OnlineUserList){
            qDebug()<<"SendFileData";
            QString userList(arr);
            signOnlineUserList(userList);
        }
        delete [] rbytes;
        buffer = m_buffer.right(totalLen - size);
        totalLen = buffer.size();
        m_buffer = buffer;
    }
}
