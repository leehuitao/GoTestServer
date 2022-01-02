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

    void setFileData(QString filePath);
protected:
    void run() override;

signals:
    void signSendFileData(FileBody );
public slots:
    void slotContinue(int ,FileBody );
private:
    QString             m_filePath;
    QString             m_fileMd5;
    int                 m_fileTotalSize;
    int                 m_fileCurrentSize;
    QMutex              m_mutex;
    QWaitCondition      m_condition;
    FileBody            m_recvFileBody;
    int                 m_method=0;
    QFile *             m_file;
};

#endif // FILETHREAD_H
