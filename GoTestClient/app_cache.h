#ifndef APPCACHE_H
#define APPCACHE_H

#include <QMap>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QMutexLocker>
#include <QVector>

static QString getCurrentTime(){
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd-hh-mm-ss-zzz");
    return current_date;
}

struct UserStruct{
    QString  UserName       ;
    QString  UserLoginName  ;
    int      ParentDeptID   ;
};

struct DeptStruct{
    QString     DeptName     ;
    int         DeptID       ;
    int         ParentDeptID ;
    int         Level        ;
};
class AppCache: public QObject
{
public:

    Q_OBJECT
public:
    static AppCache* Instance(){
        static QMutex mutex;
        if (!instance_) {
            QMutexLocker locker(&mutex);
            if (!instance_) {
                instance_ = new AppCache;
            }
        }
        return instance_;
    }
    QString m_userName = "test";
    int     m_msgSize=0;

private:
    AppCache();
private:
    static AppCache*        instance_;
};

#endif // APPCACHE_H
