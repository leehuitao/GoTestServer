#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <QImage>
#include <QObject>
#include <QPixmap>
#include <QTcpServer>
#include <QTcpSocket>
#include "packet_process.h"
#include "../File/file_thread.h"

class TcpClient: public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

signals:
    void signLoginStatus(int status,QString str = "");

    void signLoginBody(LoginBody);

    void signOnlineUserList(QString userList);

    void signOnlineUserUpdate(OnlineListBody body);
    //500ms 向界面发送一次包防止一直刷新卡顿
    void signRecvMsgList(QList<MsgBody>);

    void signRecvMsg(MsgBody);
    //发送到 文件线程
    void signRecvFile(FileBody);

    void signContinueSendFile(int,FileBody);

    void signSendFileProgress(int totalsize,int currentsize);

    void signRecvFileProgress(int totalsize,int currentsize,int sendstatus);

    void signRecvFileCompelte(QString filename ,QString type);
public slots:
    void sendLogin(QString  ip,int port ,LoginBody body);

    void sendLogout(LoginBody body);

    void sendMsg(MsgBody body, int method ,int methodType);

    void sendFile(FileBody body, int method ,int methodType);
private slots:
    void init();

    void connected();

    void slotDisconnect(QAbstractSocket::SocketError socketError);

    void receiveData();

    void sendFileData(int method ,FileBody body );

    void continueRecv(FileBody);

private:
    QTcpSocket *            m_socket = nullptr;
    QList<MsgBody>          m_MsgCache;
    QByteArray              m_buffer;
    PacketProcess           m_packProcess;
    QString                 m_serverip;
    int                     m_serverport;
    LoginBody               m_loginBody;
    FileThread              *m_fileThreadList=nullptr;
    QFile   *               m_recvFile;
    FileBody                m_FileBodyCache;
    int                     m_totalSize=0;
    int                     m_currentSize=0;
};

#endif // TCPCLIENT_H
