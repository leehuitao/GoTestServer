#include "file_thread.h"
//每次发送的文件包大小32k
#define FileBufferSize 1024*32

FileThread::FileThread(QObject *parent) : QThread(parent)
{

}

void FileThread::setFileData(QString filePath)
{
    m_filePath = filePath;
    m_fileMd5 = createFileMd5(filePath);
}

void FileThread::run()
{
    //启动发送文件第一个包
    FileBody body;
    signSendFileData(body);
    m_file = new QFile(m_filePath);

    while(1){
        m_mutex.lock();//等待有新包进来再执行
        m_condition.wait(&m_mutex);
        m_mutex.unlock();
        if(m_method == SendFileData){//继续发送文件

        } else{
            m_file->close();
            return;
        }
    }
}

void FileThread::slotContinue(int method, FileBody body)
{
    m_mutex.lock();
    m_recvFileBody = body;
    m_method = method;
    m_mutex.unlock();
    m_condition.notify_all();
}
