#include "message_box_widget.h"
#include "ui_message_box_widget.h"
#include "app_cache.h"
MessageBoxWidget::MessageBoxWidget(MsgBody body,int w, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageBoxWidget)
{
    ui->setupUi(this);
    this->setMaximumWidth(w);
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

void MessageBoxWidget::create(MsgBody body)
{
    if(body.UserName == AppCache::Instance()->m_userName){
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
