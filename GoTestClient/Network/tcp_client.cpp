#include "tcp_client.h"

TcpClient::TcpClient(QObject *parent)
{
    init();
}

void TcpClient::sendLogin(QString ip, int port, LoginBody body)
{
    m_socket->connectToHost(QHostAddress(ip),port);
    connect(m_socket,&QTcpSocket::connected,[=](LoginBody body){//连接成功以后发送登录包
        qDebug()<<"tcp connected";
        Pack pack(body,Login,0);
        auto data = pack.toByte();
        m_socket->write(data);
    });
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
    m_socket = new QTcpSocket;
}

void TcpClient::slotDisconnect(QAbstractSocket::SocketError socketError)
{

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
        if(method == Login){
            LoginBody body;
            m_packProcess.parsePack(arr,size,method,methodType,&body,method);
            signLoginStatus(body.LoginStatus,body.LoginStatus ? "登录成功":"登录失败");

        }else if(method == Msg){
            MsgBody body;
            m_packProcess.parsePack(arr,size,method,methodType,&body,method);

            signRecvMsg(body);
        }else if(method == SendFileData){
            qDebug()<<"SendFileData";
        }
        delete [] rbytes;
        buffer = m_buffer.right(totalLen - size);
        totalLen = buffer.size();
        m_buffer = buffer;
    }
}
