#include "file_thread.h"
#include <QDebug>
//每次发送的文件包大小32k
#define FileBufferSize 1024*8

FileThread::FileThread(QObject *parent) : QThread(parent)
{

}

void FileThread::setFileData(FileBody body)
{
    m_filePath = body.FileName;
    m_fileMd5 = body.FileMD5;
    m_dstUser = body.DstUserName;
    m_userName = body.UserName;
    m_userLoginName = body.UserLoginName;
}

void FileThread::close()
{
    m_mutex.lock();
    m_fileCurrentSize = 0;
    m_method = SendFileSuccess;
    m_mutex.unlock();
    m_condition.notify_all();
}

void FileThread::run()
{
    //启动发送文件第一个包
    m_file = new QFile(m_filePath);
    m_file->open(QFile::ReadOnly);
    FileBody body;
    body.UserName = m_userName;
    body.UserLoginName = m_userLoginName;
    body.FileName = m_filePath.split("/").last();
    body.DstUserName = m_dstUser;
    body.FileMD5 = m_fileMd5;
    body.TotalSize = m_file->size();
    body.CurrentSize = 0;
    signSendFileData(StartSendFile,body);
    qDebug()<<"send frist file packet";
    while(1){
        m_mutex.lock();//等待有新包进来再执行
        m_condition.wait(&m_mutex);
        m_mutex.unlock();
        if(m_method == ContinueSendFileData && m_fileCurrentSize != body.TotalSize){//继续发送文件
            m_file->seek(m_fileCurrentSize);
            body.FileData = m_file->read(FileBufferSize);
            m_fileCurrentSize += body.FileData.size();
            body.CurrentSize = m_fileCurrentSize;
            signSendFileData(ContinueSendFileData,body);
        }else if (m_fileCurrentSize == body.TotalSize) {
            qDebug()<<"send file ok";
            body.FileData = QByteArray();
            body.CurrentSize = m_fileCurrentSize;
            body.SendStatus  = 1;
            signSendFileData(SendFileSuccess,body);
            m_file->close();
            return;
        }else{
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
