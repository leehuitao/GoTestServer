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
