#include "mainwindow.h"

#include <QApplication>

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
