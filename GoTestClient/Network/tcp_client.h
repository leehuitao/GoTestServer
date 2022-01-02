#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <QImage>
#include <QObject>
#include <QPixmap>
#include <QTcpServer>
#include <QTcpSocket>
#include "packet_process.h"
#define HeaderSize 12

class TcpClient: public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

signals:
    void signLoginStatus(int status,QString str = "");
    //500ms 向界面发送一次包防止一直刷新卡顿
    void signRecvMsgList(QList<MsgBody>);

    void signRecvMsg(MsgBody);
    //发送到 文件线程
    void signRecvFile(FileBody);
public slots:
    void sendLogin(QString  ip,int port ,LoginBody body);

    void sendMsg(MsgBody body, int method ,int methodType);

    void sendFile(FileBody body, int method ,int methodType);
private slots:
    void init();

    void slotDisconnect(QAbstractSocket::SocketError socketError);

    void receiveData();

private:
    QTcpSocket *    m_socket;
    QList<MsgBody>  m_MsgCache;
    QByteArray      m_buffer;
    PacketProcess   m_packProcess;
};

#endif // TCPCLIENT_H
