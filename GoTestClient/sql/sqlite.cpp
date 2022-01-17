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
    query.exec(QString(insertMsg).arg(BelongUser).arg(SendUser).arg(RecvUser).arg(Content).arg(SendTime).arg(MsgType));

}

HistoryMsgList Sqlite::selectHistoryMsg(QString BelongUser)
{
    QSqlQuery query;
    query.exec(QString(selectMsg).arg(BelongUser));
    HistoryMsgList list;
    while(query.next()){
        HistoryMsgStruct msg;
        msg.id = query.value(0).toInt();
        msg.Belong2User = query.value(0).toString();
        msg.SendUser = query.value(0).toString();
        msg.RecvUser = query.value(0).toString();
        msg.Content = query.value(0).toString();
        msg.SendTime = query.value(0).toString();
        msg.MsgType = query.value(0).toInt();
        list.insert(0,msg);
    }
    return list;
}

void Sqlite::createDafultTable()
{
    QSqlQuery query;
    query.exec(HistoryMsg);
    qDebug()<<query.lastError();
}
