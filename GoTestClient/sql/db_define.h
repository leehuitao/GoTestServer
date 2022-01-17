#ifndef DB_DEFINE_H
#define DB_DEFINE_H
#include <QObject>
#define HistoryMsg "CREATE TABLE 'HistoryMsg' (" \
    "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,        "\
    "Belong2User TEXT NOT NULL,                            "\
    "SendUser TEXT NOT NULL,                               "\
    "RecvUser TEXT NOT NULL,                               "\
    "Content TEXT NOT NULL,                                "\
    "SendTime TEXT,                                        "\
    "MsgType integer                                       "\
    ");"
#define insertMsg "insert into HistoryMsg (Belong2User,SendUser,RecvUser,Content,SendTime,MsgType) values ('%1','%2','%3','%4','%5',%6)"
#define selectMsg "select * from HistoryMsg where Belong2User = %1 order by id DESC limit 20;"
struct HistoryMsgStruct{
    int             id;
    QString         Belong2User;
    QString         SendUser;
    QString         RecvUser;
    QString         Content;
    QString         SendTime;
    int             MsgType;
};


typedef QList<HistoryMsgStruct> HistoryMsgList;

#endif // DB_DEFINE_H
