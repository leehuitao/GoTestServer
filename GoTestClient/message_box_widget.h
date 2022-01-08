#ifndef MESSAGE_BOX_WIDGET_H
#define MESSAGE_BOX_WIDGET_H

#include <QWidget>
#include "Network/packet_define.h"
namespace Ui {
class MessageBoxWidget;
}

class MessageBoxWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageBoxWidget(MsgBody body,int w,QWidget *parent = nullptr);
    ~MessageBoxWidget();
    void setW(int w);
private:
    void create(MsgBody body);

private:
    Ui::MessageBoxWidget *ui;
};

#endif // MESSAGE_BOX_WIDGET_H
