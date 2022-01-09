#include "message_box_widget.h"
#include "ui_message_box_widget.h"
#include "app_cache.h"

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QMenu>
#include <QProcess>
#include <QTextCodec>
MessageBoxWidget::MessageBoxWidget(MsgBody body, int w, int msgType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageBoxWidget),
    m_msgType(msgType)
{
    ui->setupUi(this);
    this->setMaximumWidth(w);
    m_isSelfMsg = (body.UserName == AppCache::Instance()->m_userName);
    create(body);
}

MessageBoxWidget::~MessageBoxWidget()
{
    delete ui;
}

void MessageBoxWidget::setW(int w)
{
    this->setMaximumWidth(w);
}

void MessageBoxWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if(m_msgType){
        QMenu *m_contextMenu = new QMenu;
        QAction *m_openAction = new QAction("打开文件位置",this);
        m_contextMenu->addAction(m_openAction);
        connect(m_openAction,&QAction::triggered,this,&MessageBoxWidget::openFilePath);
        QPoint i = event->pos();
        if(m_isSelfMsg){
            QRect rect(ui->label_4->pos().x(),ui->label_4->pos().y(),ui->label_4->width(),ui->label_4->height());
            if (rect.contains(i))
            {
                m_contextMenu->exec(event->globalPos());
            }

        }else{

            QRect rect(ui->label_2->pos().x(),ui->label_2->pos().y(),ui->label_2->width(),ui->label_2->height());
            if (rect.contains(i))
            {
                m_contextMenu->exec(event->globalPos());
            }
        }
    }
}

void MessageBoxWidget::openFilePath(int status)
{
    if(m_isSelfMsg){
        QString path =QCoreApplication::applicationDirPath();
        QString filename = ui->label_4->text().split(":").last();
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }else{
        QString path = QCoreApplication::applicationDirPath();
        QString filename = ui->label_2->text().split(":").last();
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }
}

void MessageBoxWidget::create(MsgBody body)
{
    if(m_isSelfMsg){
        ui->other->hide();
        ui->label_4->setText(body.Msg);
        ui->label_3->setPixmap(QPixmap("F:/yiheng/GoTestServer/GoTestClient/resource/self.png"));
        int h = ui->label_4->height();
        //this->setMaximumHeight(h+18);

    }else{
        ui->self->hide();
        ui->label_2->setText(body.Msg);
        ui->label_1->setPixmap(QPixmap("F:/yiheng/GoTestServer/GoTestClient/resource/other.png"));
        int h = ui->label_2->height();
        //this->setMaximumHeight(h+18);
    }
}
