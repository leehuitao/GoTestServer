#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Network/tcp_client.h"
#include "emotion_widget.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUserName(QString);
signals:
    void signLogin(QString  ip,int port ,LoginBody body);

    void signLogout(LoginBody body);

    void signSendMsg(MsgBody body, int method ,int methodType);

    void signSendFile(FileBody body, int method ,int methodType);

    void signGetOrg(SystemBody body, int method ,int methodType);

    void signOnlineUsers(SystemBody body, int method ,int methodType);
private slots:
    void on_login_btn_clicked();

    void on_sendmsg_btn_clicked();

    void on_file_select_btn_clicked();

    void on_file_send_btn_clicked();

    void slotLoginStatus(int status,QString str);

    void slotRecvMsg(MsgBody);

    void slotRecvFileProgress(int totalsize,int currentsize,int sendstatus);

    void slotSendFileProgress(int totalsize,int currentsize);

    void slotRecvOnlineUserList(QString userList);

    void slotOnlineUserUpdate(OnlineListBody body);

    QString getCurrentTimeSeconds();

    QString getHostMacAddress();

    void on_listWidget_currentTextChanged(const QString &currentText);

    void slotRecvFileCompelte(QString filename,QString type);

    void slotLoginBody(LoginBody);

    void on_emoji_btn_clicked();

    void slotGetOrg(QJsonDocument body);

private:
    void init();
    void setBottom();
    void initDB();
private:
    Ui::MainWindow *ui;
    TcpClient   *   m_tcpClient;
    int             m_loginStatus = 0;
    QString         m_currentChoiseUser;
    QString         m_userName;
    EmotionWidget * m_emojiWidget;
};
#endif // MAINWINDOW_H
