#ifndef FILETHREAD_H
#define FILETHREAD_H

#include <QObject>
#include <QThread>
#include "../Network/packet_define.h"
#include <QMutex>
#include <QWaitCondition>

class FileThread : public QThread
{
    Q_OBJECT
public:
    explicit FileThread(QObject *parent = nullptr);

    void setFileData(FileBody);
protected:
    void run() override;
signals:
    void signSendFileData(int,FileBody);
public slots:
    void slotContinue(int,FileBody );
private:
    QString             m_filePath;
    QString             m_fileMd5;
    int                 m_fileTotalSize;
    int                 m_fileCurrentSize = 0;
    QMutex              m_mutex;
    QWaitCondition      m_condition;
    FileBody            m_recvFileBody;
    int                 m_method=0;
    QFile *             m_file;
    QString             m_dstUser;
    QString             m_userName;
};

#endif // FILETHREAD_H
