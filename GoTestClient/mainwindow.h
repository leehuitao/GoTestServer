#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Network/tcp_client.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void signLogin(QString  ip,int port ,LoginBody body);

    void signLogout(LoginBody body);

    void signSendMsg(MsgBody body, int method ,int methodType);

    void signSendFile(FileBody body, int method ,int methodType);
private slots:
    void on_login_btn_clicked();

    void on_sendmsg_btn_clicked();

    void on_file_select_btn_clicked();

    void on_file_send_btn_clicked();

    void slotLoginStatus(int status,QString str);

    void slotRecvMsg(MsgBody);

    void slotRecvFileProgress(int totalsize,int currentsize,int sendstatus);

    void slotRecvOnlineUserList(QString userList);

    QString getCurrentTimeSeconds();

    QString getHostMacAddress();

    void on_listWidget_currentTextChanged(const QString &currentText);

private:
    void init();

private:
    Ui::MainWindow *ui;
    TcpClient   *   m_tcpClient;
    int             m_loginStatus = 0;
};
#endif // MAINWINDOW_H
