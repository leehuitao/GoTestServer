#ifndef SQLITE_H
#define SQLITE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "db_define.h"
class Sqlite : public QObject
{
    Q_OBJECT
public:
    explicit Sqlite(QObject *parent = nullptr);

    void initDB();

    void insertHistoryMsg(QString BelongUser,QString SendUser,QString RecvUser,QString Content,QString SendTime,int MsgType);

    HistoryMsgList selectHistoryMsg(QString BelongUser);
signals:

public slots:

private:
    QSqlDatabase m_database;

private:
    void createDafultTable();
};

#endif // SQLITE_H
