#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QFileDialog>
#include <QNetworkInterface>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_login_btn_clicked()
{
    if(m_loginStatus == 0){
        QString ip = ui->ip->text();
        int port   = ui->port->text().toInt();
        LoginBody body;
        body.UserName   = "test";
        body.PassWord   = "test";
        body.LoginTime  = getCurrentTimeSeconds();
        body.MacAddress = getHostMacAddress();
        body.Notice     = 0;
        signLogin(ip,port,body);
    }else{
        LoginBody body;
        body.UserName   = "test";
        signLogout(body);
    }

}

void MainWindow::on_sendmsg_btn_clicked()
{
    MsgBody body;
    body.UserName   = "test";
    body.DstUser    = m_currentChoiseUser;
    body.DstUserID  = 123;
    body.Msg        = ui->msg_send_edit->text();
    body.MsgType    = 0;
    signSendMsg(body,MsgMethod,0);
}

void MainWindow::on_file_select_btn_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(QStringLiteral("选中文件"));
    fileDialog->setDirectory(".");
    fileDialog->setNameFilter(tr("File(*.*)"));
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }
    if(fileNames.size()>0)
        ui->file_send_edit->setText(fileNames.at(0));
}

void MainWindow::on_file_send_btn_clicked()
{
    QString path = ui->file_send_edit->text();
    FileBody body;
    body.UserName = "test";
    body.FileName = path;
    body.DstUserName = m_currentChoiseUser;
    body.FileMD5 = createFileMd5(path);
    signSendFile(body,StartSendFile,0);
}

void MainWindow::slotLoginStatus(int status, QString str)
{
    m_loginStatus = status;
    if(status){
        ui->login_status_lab->setStyleSheet("background-color: rgb(0, 255, 0);");
        ui->login_btn->setText("quit");
    }else{
        ui->login_status_lab->setStyleSheet("background-color: rgb(255, 0, 0);");
        ui->login_btn->setText("login");
    }
    ui->login_status_str_lab->setText(str);
}

void MainWindow::slotRecvMsg(MsgBody body)
{
    qDebug()<<__FUNCTION__<<body.Msg;
}

void MainWindow::slotRecvFileProgress(int totalsize, int currentsize, int sendstatus)
{

}

void MainWindow::slotRecvOnlineUserList(QString userList)
{
    qDebug()<<__FUNCTION__<<userList;
    ui->listWidget->clear();
    QStringList list = userList.split(",");
    for(auto it : list){
        if(!it.isEmpty()){
            ui->listWidget->addItem(it);
        }
    }
}

void MainWindow::slotOnlineUserUpdate(OnlineListBody body)
{
    if(body.Status == UserLoginStatus){
        ui->listWidget->addItem(body.UserName);
    }else if(body.Status == UserLogoffStatus){
        auto item = ui->listWidget->findItems(body.UserName,Qt::MatchExactly);
        if(item.size() == 1){
            ui->listWidget->removeItemWidget(item.at(0));
            delete item.at(0);
        }
    }
}

QString MainWindow::getCurrentTimeSeconds()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd-hh-mm-ss");
    return current_date;
}

QString MainWindow::getHostMacAddress()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++)
    {
        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;
}


void MainWindow::init(){
    m_tcpClient = new TcpClient;
    QThread *t1 = new QThread;
    connect(this,&MainWindow::signLogin,                m_tcpClient,&TcpClient::sendLogin                 ,Qt::QueuedConnection);
    connect(this,&MainWindow::signLogout,               m_tcpClient,&TcpClient::sendLogout                ,Qt::QueuedConnection);
    connect(this,&MainWindow::signSendMsg,              m_tcpClient,&TcpClient::sendMsg                   ,Qt::QueuedConnection);
    connect(this,&MainWindow::signSendFile,             m_tcpClient,&TcpClient::sendFile                  ,Qt::QueuedConnection);

    connect(m_tcpClient ,&TcpClient::signLoginStatus,       this,&MainWindow::slotLoginStatus               ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signRecvMsg,           this,&MainWindow::slotRecvMsg                   ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signRecvFileProgress,  this,&MainWindow::slotRecvFileProgress          ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signOnlineUserList,    this,&MainWindow::slotRecvOnlineUserList        ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signOnlineUserUpdate,  this,&MainWindow::slotOnlineUserUpdate          ,Qt::QueuedConnection);

    m_tcpClient->moveToThread(t1);
    t1->start();
}

void MainWindow::on_listWidget_currentTextChanged(const QString &currentText)
{
    qDebug()<<__FUNCTION__<<currentText<<" clicked ";
    m_currentChoiseUser = currentText;
}
