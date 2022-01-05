#include "mainwindow.h"

#include <QApplication>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

void registerStruct(){
    qRegisterMetaType<LoginBody>("LoginBody");
    qRegisterMetaType<FileBody>("FileBody");
    qRegisterMetaType<MsgBody>("MsgBody");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    registerStruct();
    MainWindow w;
    w.show();
    return a.exec();
}
