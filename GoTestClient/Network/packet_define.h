#ifndef PACKET_DEFINE_H
#define PACKET_DEFINE_H

#include <QDir>
#include <QObject>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDataStream>
#define     Login 			 100
#define     Logout 			 101
#define     Msg				 102
#define     StartSendFile	 103
#define     SendFileData	 104
#define     SendFileCancel	 105
#define     SendFileSuccess	 106

#define HeaderSize 12
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
    int         LoginStatus = 0;
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
    Pack(LoginBody body, int method ,int methodType){
        QJsonObject json;//构建json对象json
        json.insert("UserName", body.UserName);
        json.insert("PassWord", body.PassWord);
        json.insert("Notice", body.Notice);
        json.insert("MacAddress", body.MacAddress);
        json.insert("LoginTime", body.LoginTime);
        json.insert("LoginStatus", body.LoginStatus);
        QJsonDocument document;
        document.setObject(json);
        QByteArray byte_array = document.toJson(QJsonDocument::Compact);
        Body = byte_array;
        Header.Method = method;
        Header.MethodType = methodType;
        Header.PackSize =HeaderSize +   Body.size();
    }

    QByteArray toByte(){
        QByteArray m_buffer;
        QDataStream packet(&m_buffer,QIODevice::WriteOnly);
        packet<<Header.PackSize<<Header.Method<<Header.MethodType;
        m_buffer += Body.data();
        return m_buffer;
    }
};

#endif // PACKET_DEFINE_H
