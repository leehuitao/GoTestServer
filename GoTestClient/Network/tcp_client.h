#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <QImage>
#include <QObject>
#include <QPixmap>
#include <QTcpServer>
#include <QTcpSocket>
#include "packet_process.h"


class TcpClient: public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

signals:
    void signLoginStatus(int status,QString str = "");

    void signOnlineUserList(QString userList);
    //500ms 向界面发送一次包防止一直刷新卡顿
    void signRecvMsgList(QList<MsgBody>);

    void signRecvMsg(MsgBody);
    //发送到 文件线程
    void signRecvFile(FileBody);

    void signRecvFileProgress(int totalsize,int currentsize,int sendstatus);
public slots:
    void sendLogin(QString  ip,int port ,LoginBody body);

    void sendLogout();

    void sendMsg(MsgBody body, int method ,int methodType);

    void sendFile(FileBody body, int method ,int methodType);
private slots:
    void init();

    void connected();

    void slotDisconnect(QAbstractSocket::SocketError socketError);

    void receiveData();

private:
    QTcpSocket *    m_socket = nullptr;
    QList<MsgBody>  m_MsgCache;
    QByteArray      m_buffer;
    PacketProcess   m_packProcess;
    QString         m_serverip;
    int             m_serverport;
    LoginBody       m_loginBody;
};

#endif // TCPCLIENT_H
