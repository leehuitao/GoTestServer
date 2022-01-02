#ifndef PACKET_DEFINE_H
#define PACKET_DEFINE_H

#include <QDir>
#include <QObject>
#include <QCryptographicHash>
#define     Login 			 100
#define     Logout 			 101
#define     Msg				 102
#define     StartSendFile	 103
#define     SendFileData	 104
#define     SendFileCancel	 105
#define     SendFileSuccess	 106
static QString createFileMd5(QString filePath){
    QString sMd5;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray bArray = QCryptographicHash::hash(file.readAll(),QCryptographicHash::Md5);
        sMd5 = QString(bArray.toHex()).toUpper();
    }
    file.close();
    return sMd5;
}

static QString createUuid(){

}

struct MsgBody  {
    QString     User;
    int         UserID;
    int         MsgType;
    QString     UserMsg;
};

struct FileBody  {
    QString     FileName      ;
    QString     FileMD5       ;
    int         TotalSize     ;
    int         CurrentSize   ;
    int         DstUserID     ;
    int         SendStatus    ;
    QByteArray  FileData      ;
};

// LoginBody
struct LoginBody  {
    QString     UserName   ;
    QString     PassWord   ;
    bool        Notice     ;
    QString     MacAddress ;
    QString     LoginTime  ;
    int         LoginStatus;
};

// Header 协议头
struct Header  {
    //包大小
    int PackSize ;
    //指令
    int Method ;
    //指令类别
    int MethodType;
};

// Pack 协议体
struct Pack  {
    Header Header;
    //数据体
    QByteArray  Body;

    Pack(MsgBody body, int method ,int methodType){}
    Pack(FileBody body, int method ,int methodType){}
    Pack(LoginBody body, int method ,int methodType){}

    QByteArray toByte(){}
};

#endif // PACKET_DEFINE_H
