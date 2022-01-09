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
    explicit MessageBoxWidget(MsgBody body,int w,int msgType = 0,QWidget *parent = nullptr);
    ~MessageBoxWidget();
    void setW(int w);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
private:
    void create(MsgBody body);

    void openFilePath(int );

private:
    Ui::MessageBoxWidget *ui;

    int         m_msgType;

    bool        m_isSelfMsg;
};

#endif // MESSAGE_BOX_WIDGET_H
