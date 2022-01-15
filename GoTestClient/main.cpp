#include "mainwindow.h"

#include <QApplication>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

void registerStruct(){
    qRegisterMetaType<LoginBody>("LoginBody");
    qRegisterMetaType<FileBody>("FileBody");
    qRegisterMetaType<MsgBody>("MsgBody");
    qRegisterMetaType<OnlineListBody>("OnlineListBody");
    qRegisterMetaType<SystemBody>("SystemBody");



}

int main(int argc, char *argv[])
{

    qDebug() << "argc:" <<argc;
    QApplication a(argc, argv);
    registerStruct();
    MainWindow w;
    if(argc == 2)
    {
        w.setUserName(argv[1]);
    }
    w.show();
    return a.exec();
}
