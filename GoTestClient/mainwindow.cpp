#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QDateTime>
#include <QFileDialog>
#include <QNetworkInterface>
#include <QScrollBar>
#include <QThread>
#include <QJsonDocument>
#include <QStandardItemModel>
#include "message_box_widget.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    connect(ui->scrollArea->verticalScrollBar(),&QScrollBar::rangeChanged,[=](int min,int max){
        ui->scrollArea->verticalScrollBar()->setValue(max);
    });
    initDB();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUserName(QString username)
{
    AppCache::Instance()->m_userName = username;
}


void MainWindow::on_login_btn_clicked()
{
    if(m_loginStatus == 0){
        QString ip = ui->ip->text();
        int port   = ui->port->text().toInt();
        LoginBody body;
        body.UserLoginName   = AppCache::Instance()->m_userName;
        body.PassWord   = AppCache::Instance()->m_userName;
        body.LoginTime  = getCurrentTimeSeconds();
        body.MacAddress = getHostMacAddress();
        body.Notice     = 0;
        signLogin(ip,port,body);
    }else{
        LoginBody body;
        body.UserLoginName   = AppCache::Instance()->m_userName;
        body.UserName   = AppCache::Instance()->m_userName;
        signLogout(body);
    }

}

void MainWindow::on_sendmsg_btn_clicked()
{
    MsgBody body;
    body.UserLoginName   = AppCache::Instance()->m_userName;
    body.UserName   = AppCache::Instance()->m_userName;
    body.DstUser    = m_currentChoiseUser;
    body.DstUserID  = 123;
    body.Msg        = ui->msg_send_edit->toHtml();
    qDebug()<<body.Msg;
    body.MsgType    = 0;
    signSendMsg(body,MsgMethod,0);
    MessageBoxWidget *w = new MessageBoxWidget(body,ui->scrollArea->width()-18);
    ui->verticalLayout->insertWidget(AppCache::Instance()->m_msgSize++,w);
    ui->scrollArea->ensureWidgetVisible(w);
    m_sql.insertHistoryMsg(m_currentChoiseUser,AppCache::Instance()->m_userName,m_currentChoiseUser,body.Msg,getCurrentTime(),0);
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
    body.UserLoginName   = AppCache::Instance()->m_userName;
    body.UserName = AppCache::Instance()->m_userName;
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
        SystemBody body;//登录获取组织架构
        body.UserLoginName  = AppCache::Instance()->m_userName;
        body.SystemCMD      = "0";
        signGetOrg(body,GetOrg,0);
    }else{
        ui->login_status_lab->setStyleSheet("background-color: rgb(255, 0, 0);");
        ui->login_btn->setText("login");
    }
    ui->login_status_str_lab->setText(str);
}

void MainWindow::slotRecvMsg(MsgBody body)
{
    if(body.UserLoginName == m_currentChoiseUser){
        MessageBoxWidget *w = new MessageBoxWidget(body,ui->scrollArea->width()-18);
        ui->verticalLayout->insertWidget(AppCache::Instance()->m_msgSize++,w);
    }
    qDebug()<<__FUNCTION__<<body.Msg;
    m_sql.insertHistoryMsg(body.UserLoginName,body.UserLoginName,body.DstUser,body.Msg,getCurrentTime(),0);
}

void MainWindow::slotSendFileProgress(int totalsize, int currentsize)
{
    int progress = double(currentsize)/double(totalsize)*100;
    ui->progressBar->setValue(progress);
}

void MainWindow::slotRecvFileProgress(int totalsize, int currentsize,int sendstatus)
{
    int progress = double(currentsize)/double(totalsize)*100;
    ui->recv_progressBar->setValue(progress);

}

void MainWindow::slotRecvOnlineUserList(QString userList)
{
    qDebug()<<__FUNCTION__<<userList;
    QStringList list = userList.split(",");
    for(auto it : list){
        if(!it.isEmpty()){
            if(it == AppCache::Instance()->m_userName)
                m_userName2UiPointer[it]->setIcon(QIcon(":/resource/self.png"));
            else
                m_userName2UiPointer[it]->setIcon(QIcon(":/resource/other.png"));
        }
    }
}

void MainWindow::slotOnlineUserUpdate(OnlineListBody body)
{
    if(body.Status == UserLoginStatus){
        if(body.UserLoginName == AppCache::Instance()->m_userName)
            m_userName2UiPointer[body.UserLoginName]->setIcon(QIcon(":/resource/self.png"));
        else
            m_userName2UiPointer[body.UserLoginName]->setIcon(QIcon(":/resource/other.png"));
    }else if(body.Status == UserLogoffStatus){
        m_userName2UiPointer[body.UserLoginName]->setIcon(QIcon(":/resource/offline.png"));
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
    connect(this,&MainWindow::signGetOrg,               m_tcpClient,&TcpClient::sendGetOrg                ,Qt::QueuedConnection);
    connect(this,&MainWindow::signOnlineUsers,          m_tcpClient,&TcpClient::sendGetOnlineUsers        ,Qt::QueuedConnection);

    connect(m_tcpClient ,&TcpClient::signLoginStatus,       this,&MainWindow::slotLoginStatus               ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signLoginBody,         this,&MainWindow::slotLoginBody                 ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signRecvMsg,           this,&MainWindow::slotRecvMsg                   ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signRecvFileProgress,  this,&MainWindow::slotRecvFileProgress          ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signOnlineUserList,    this,&MainWindow::slotRecvOnlineUserList        ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signOnlineUserUpdate,  this,&MainWindow::slotOnlineUserUpdate          ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signSendFileProgress,  this,&MainWindow::slotSendFileProgress          ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signRecvFileCompelte,  this,&MainWindow::slotRecvFileCompelte          ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signGetOrg,            this,&MainWindow::slotGetOrg                    ,Qt::QueuedConnection);
    connect(m_tcpClient ,&TcpClient::signGetUserOrg,        this,&MainWindow::slotGetUserOrg                    ,Qt::QueuedConnection);

    m_tcpClient->moveToThread(t1);
    t1->start();

    m_emojiWidget = new EmotionWidget;
    m_emojiWidget->initEmotion();

    connect(m_emojiWidget,&EmotionWidget::emojiClicked,[=](int number){

        QTextCursor cursor = ui->msg_send_edit->textCursor();
        QTextDocument *document = ui->msg_send_edit->document();
        //cursor.movePosition(QTextCursor::End);
        cursor.insertImage(QString(":/resource/emoji/%1.gif").arg(number+1));
    });
    m_pModel = new QStandardItemModel(ui->treeView);
    ui->treeView->header()->hide();
    ui->treeView->setModel(m_pModel);
    ui->treeView->setEditTriggers(QHeaderView::NoEditTriggers);
    ui->treeView->setIconSize(QSize(40,40));
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(clicked(QModelIndex)));
}

void MainWindow::setBottom()
{
    QScrollBar *pScrollBar = ui->scrollArea->verticalScrollBar();
    if (pScrollBar != nullptr)
    {
        int nMax = pScrollBar->maximum();
        pScrollBar->setValue(nMax);
    }
}

void MainWindow::initDB()
{
    m_sql.initDB();

}

void MainWindow::drawOrg(QJsonDocument json)
{
    m_pModel->clear();
    m_deptListMap.clear();
    m_id2UiPointer.clear();
    m_userName2UiPointer.clear();
    m_userListMap.clear();
    auto jsonobj = json.object();
    auto keys    = jsonobj.keys();
    for(auto it : keys){
        auto jval = jsonobj.value(it);

        QByteArray details = QByteArray::fromBase64(jval.toString().toLocal8Bit().data());

        qDebug()<<"jval 1"<<details;
        QJsonParseError jsonError;
        QJsonDocument jsonDoc(QJsonDocument::fromJson(details, &jsonError));
        if(jsonError.error != QJsonParseError::NoError)
        {
            qDebug() << "json error!" << jsonError.errorString();
            return;
        }
        auto values = jsonDoc.object();
        QString deptName = values.value("DeptName").toString();
        int DeptID = values.value("DeptID").toInt();
        int ParentDeptID = values.value("ParentDeptID").toInt();
        int Level = values.value("Level").toInt();
        DeptStruct d;
        d.DeptName = deptName;
        d.DeptID = DeptID;
        d.ParentDeptID = ParentDeptID;
        d.Level = Level;
        qDebug()<<"deptName = "<<deptName<<"DeptID = "<<DeptID<<"ParentDeptID = "<<ParentDeptID<<"Level = "<<Level;
        m_deptListMap[Level].append(d);
    }
    for(auto it : m_deptListMap){
        for(auto item : it){
            auto group = new QStandardItem(item.DeptName);
            m_id2UiPointer[item.DeptID] = group;
            if(item.ParentDeptID != 0){
                m_id2UiPointer[item.ParentDeptID]->appendRow(group);
            }else{
                m_pModel->appendRow(group);
            }
        }
    }
}

void MainWindow::drawUserOrg(QJsonDocument json)
{
    auto jsonobj = json.object();
    auto keys    = jsonobj.keys();
    for(auto it : keys){
        auto jval = jsonobj.value(it);

        QByteArray details = QByteArray::fromBase64(jval.toString().toLocal8Bit().data());

        qDebug()<<"jval 1"<<details;
        QJsonParseError jsonError;
        QJsonDocument jsonDoc(QJsonDocument::fromJson(details, &jsonError));
        if(jsonError.error != QJsonParseError::NoError)
        {
            qDebug() << "json error!" << jsonError.errorString();
            return;
        }
        auto values = jsonDoc.object();
        QString UserName = values.value("UserName").toString();
        QString UserLoginName = values.value("UserLoginName").toString();
        int     ParentDeptID = values.value("ParentDeptID").toInt();
        qDebug()<<"UserName = "<<UserName<<"UserLoginName = "<<UserLoginName
               <<"ParentDeptID = "<<ParentDeptID;
        UserStruct u;
        u.UserName = UserName;
        u.UserLoginName = UserLoginName;
        u.ParentDeptID = ParentDeptID;
        m_userListMap[ParentDeptID].append(u);
    }
    for(auto it : m_userListMap){
        for(auto user : it){
            auto item = new QStandardItem();
            item->setIcon(QIcon(":/resource/offline.png"));
            item->setText(user.UserName);
            item->setSizeHint(QSize(200,50));
            item->setWhatsThis(user.UserLoginName);
            m_id2UiPointer[user.ParentDeptID]->appendRow(item);
            m_userName2UiPointer[user.UserLoginName] = item;
        }
    }
}
void MainWindow::clicked(const QModelIndex &index)
{
    QList<MessageBoxWidget*> tlist = this->findChildren<MessageBoxWidget*>(QString("MessageBoxWidget"), Qt::FindChildrenRecursively);
    foreach(MessageBoxWidget* marker, tlist)
    {
        if(marker){
            marker->deleteLater();
            marker = nullptr;
        }
    }
    AppCache::Instance()->m_msgSize = 0;
    m_currentChoiseUser = index.data(Qt::WhatsThisRole).toString();
    auto list = m_sql.selectHistoryMsg(m_currentChoiseUser);
    for(auto it : list){
        MsgBody body(it);
        MessageBoxWidget *w = new MessageBoxWidget(body,ui->scrollArea->width()-18,body.MsgType);
        ui->verticalLayout->insertWidget(AppCache::Instance()->m_msgSize++,w);
    }
    qDebug()<<index.data()<<index.data(Qt::WhatsThisRole);
}
void MainWindow::on_listWidget_currentTextChanged(const QString &currentText)
{
    qDebug()<<__FUNCTION__<<currentText<<" clicked ";
    m_currentChoiseUser = currentText;
}

void MainWindow::slotRecvFileCompelte(QString filename, QString UserLoginName)
{
    MsgBody body;
    body.UserLoginName = UserLoginName;
    body.Msg = "接收文件成功:"+filename;
    if(UserLoginName == m_currentChoiseUser){
        MessageBoxWidget *w = new MessageBoxWidget(body,ui->scrollArea->width()-18,1);
        ui->verticalLayout->insertWidget(AppCache::Instance()->m_msgSize++,w);
    }
    m_sql.insertHistoryMsg(body.UserLoginName,body.UserLoginName,AppCache::Instance()->m_userName,body.Msg,getCurrentTime(),1);
}

void MainWindow::slotLoginBody(LoginBody body)
{
    ui->username_lab->setText(body.UserName);
}

void MainWindow::on_emoji_btn_clicked()
{
    if(!m_emojiWidget->isVisible()){
        m_emojiWidget->move(ui->emoji_btn->x()+this->x()-(m_emojiWidget->width()*0.5),ui->emoji_btn->y()+this->y()-m_emojiWidget->height()+23);

        m_emojiWidget->show();
    }else{
        m_emojiWidget->hide();
    }

}

void MainWindow::slotGetOrg(QJsonDocument json)
{
    qDebug()<<json;

    //获取组织架构后更新人员组织
    SystemBody body;
    body.UserLoginName  = AppCache::Instance()->m_userName;
    body.SystemCMD      = "0";
    drawOrg(json);
    signGetOrg(body,GetUserOrg,0);

}

void MainWindow::slotGetUserOrg(QJsonDocument json)
{
    qDebug()<<json;
    //获取人员组织后更新在线人员
    SystemBody body;
    body.UserLoginName  = AppCache::Instance()->m_userName;
    body.SystemCMD      = "0";
    drawUserOrg(json);
    signGetOrg(body,GetOnlineUser,0);
}
