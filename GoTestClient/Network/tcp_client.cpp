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
    if(m_fileThreadList != nullptr){
        disconnect(this,&TcpClient::signContinueSendFile,m_fileThreadList,&FileThread::slotContinue);
        disconnect(m_fileThreadList,&FileThread::signSendFileData,this,&TcpClient::sendFileData);
        m_fileThreadList->close();// 信号量阻塞退出
        m_fileThreadList->quit();  //线程正常退出
        m_fileThreadList->wait();
        delete m_fileThreadList;
        m_fileThreadList = nullptr;
    }
    m_fileThreadList = new FileThread();
    m_fileThreadList->setFileData(body);
    connect(this,&TcpClient::signContinueSendFile,m_fileThreadList,&FileThread::slotContinue,Qt::QueuedConnection);
    connect(m_fileThreadList,&FileThread::signSendFileData,this,&TcpClient::sendFileData,Qt::QueuedConnection);
    m_fileThreadList->start();
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
            signLoginBody(body);
        }else if(method == Logout){
            signLoginStatus(0," user logout");
        }else if(method == MsgMethod){
            MsgBody body;
            body = m_packProcess.parseMsgPack(arr);
            signRecvMsg(body);
        }else if(method == StartSendFile){
            qDebug()<<"StartSendFile";
            FileBody body;
            body = m_packProcess.parseFileDataPack(arr);
            m_recvFile = new QFile(body.FileName);
            bool status = m_recvFile->open(QFile::WriteOnly|QFile::Append|QFile::Truncate);
            if(status){
                qDebug()<<"file start recv "<<body.FileName;
                m_FileBodyCache = body;
                continueRecv(m_FileBodyCache);
                m_totalSize = body.TotalSize;
                signRecvFileProgress(m_totalSize,0,1);
            }else{
                qDebug()<<"file start recv error "<<body.FileName;
            }
        }else if(method == ContinueGetFile){
            continueRecv(m_FileBodyCache);
            m_currentSize += arr.size();
            m_recvFile->write(rbytes,arr.size());
            qDebug()<<"ContinueGetFile file write size = "<<size-HeaderSize;
            signRecvFileProgress(m_totalSize,m_currentSize,1);
        }else if(method == SendFileSuccess){
            m_recvFile->write(rbytes,arr.size());
            m_currentSize = 0;
            signRecvFileProgress(m_totalSize,m_totalSize,1);
            m_recvFile->close();
            signRecvFileCompelte(m_FileBodyCache.FileName,m_FileBodyCache.UserName);
            qDebug()<<"SendFileSuccessfile write size = "<<size-HeaderSize;
        }else if(method == ContinueSendFileData){
            FileBody body;
            body = m_packProcess.parseFileDataPack(arr);
            signContinueSendFile(ContinueSendFileData,body);
            qDebug()<<"SendFileData";
        }else if(method == OnlineUserList){
            qDebug()<<"OnlineUserList";
            QString userList(arr);
            signOnlineUserList(userList);
        }else if(method == UpdateOnlineUser){
            qDebug()<<"UpdateOnlineUser";
            QString userList(arr);
            OnlineListBody body;
            body = m_packProcess.parseOnlineListBodyPack(arr);
            signOnlineUserUpdate(body);
        }
        delete [] rbytes;
        buffer = m_buffer.right(totalLen - size);
        totalLen = buffer.size();
        m_buffer = buffer;
    }
}

void TcpClient::sendFileData(int method, FileBody body)
{
    if(method == ContinueSendFileData){
        Pack pack(body,method,0,1);
        auto data = pack.toByte();
        m_socket->write(data);
        signSendFileProgress(body.TotalSize,body.CurrentSize);
    }else if(method == SendFileSuccess){
        Pack pack(body,method,0);
        auto data = pack.toByte();
        m_socket->write(data);
        signSendFileProgress(body.TotalSize,body.CurrentSize);
    }else {
        Pack pack(body,method,0);
        auto data = pack.toByte();
        m_socket->write(data);
    }

}

void TcpClient::continueRecv(FileBody body)
{
    Pack pack(body,ContinueGetFile,0);
    auto data = pack.toByte();
    m_socket->write(data);
}
