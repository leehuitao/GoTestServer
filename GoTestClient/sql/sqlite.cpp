#include "sqlite.h"
#include <QDebug>
#include <QSqlQuery>
Sqlite::Sqlite(QObject *parent) : QObject(parent)
{

}

void Sqlite::initDB()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("user.db");
    m_database.open();
    if(m_database.lastError().type() != QSqlError::NoError){
        qDebug()<<"QSqlError = "<<m_database.lastError();
    }
    createDafultTable();
}

void Sqlite::insertHistoryMsg(QString BelongUser, QString SendUser, QString RecvUser, QString Content, QString SendTime, int MsgType)
{
    QSqlQuery query;
    QString str = QString(insertMsg).arg(BelongUser).arg(SendUser).arg(RecvUser).arg(QString(QByteArray(Content.toLocal8Bit().data()).toBase64())).arg(SendTime).arg(MsgType);
    query.exec(str);
    qDebug()<<query.lastError();
}

HistoryMsgList Sqlite::selectHistoryMsg(QString BelongUser)
{
    QSqlQuery query;
    QString str = QString(selectMsg).arg(BelongUser);
    query.exec(str);
    HistoryMsgList list;
    while(query.next()){
        HistoryMsgStruct msg;
        msg.id = query.value(0).toInt();
        msg.Belong2User = query.value(1).toString();
        msg.SendUser = query.value(2).toString();
        msg.RecvUser = query.value(3).toString();
        msg.Content = QString(QByteArray::fromBase64(query.value(4).toByteArray()));
        msg.SendTime = query.value(5).toString();
        msg.MsgType = query.value(6).toInt();
        list.insert(0,msg);
    }
    qDebug()<<query.lastError()<<"str = "<<str;
    return list;
}

void Sqlite::createDafultTable()
{
    QSqlQuery query;
    query.exec(HistoryMsg);
    qDebug()<<query.lastError();
}
