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
#include <QDebug>
#define     Login                       100
#define     Logout                      101
#define     MsgMethod                   102
#define     StartSendFile               103
#define     ContinueSendFileData        104
#define     SendFileCancel              105
#define     SendFileSuccess             106
#define     OnlineUserList              200
#define     UpdateOnlineUser            201
#define HeaderSize 12

#define     UserLogoffStatus    0
#define     UserLoginStatus     1
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
    QString     UserName;
    QString     DstUser;
    int         DstUserID;
    int         MsgType;
    QString     Msg;
};

struct FileBody  {
    QString         UserName      ;
    QString         FileName      ;
    QString         FileMD5       ;
    int             TotalSize     ;
    int             CurrentSize   ;
    QString         DstUserName   ;
    int             SendStatus    ;
    QByteArray      FileData      ;
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

struct OnlineListBody  {
    QString     UserName;
    int         Status;
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

    Pack(MsgBody body, int method ,int methodType){
        QJsonObject json;//构建json对象json
        json.insert("UserName", body.UserName);
        json.insert("DstUser", body.DstUser);
        json.insert("DstUserID", body.DstUserID);
        json.insert("MsgType", body.MsgType);
        json.insert("Msg", body.Msg);
        QJsonDocument document;
        document.setObject(json);
        QByteArray byte_array = document.toJson(QJsonDocument::Compact);
        Body = byte_array;
        Header.Method = method;
        Header.MethodType = methodType;
        Header.PackSize =HeaderSize +   Body.size();
    }
    Pack(FileBody body, int method ,int methodType,bool isData = 0){
        if( isData == 0){
            QJsonObject json;//构建json对象json
            json.insert("UserName", body.UserName);
            json.insert("FileName", body.FileName);
            json.insert("FileMD5", body.FileMD5);
            json.insert("TotalSize", body.TotalSize);
            json.insert("CurrentSize", body.CurrentSize);
            json.insert("DstUserName", body.DstUserName);
            json.insert("SendStatus", body.SendStatus);
            json.insert("FileData", body.FileData.data());
            QJsonDocument document;
            document.setObject(json);
            QByteArray byte_array = document.toJson(QJsonDocument::Compact);
            Body = byte_array;
            Header.Method = method;
            Header.MethodType = methodType;
            Header.PackSize =HeaderSize +   Body.size();
        }else{
            QDataStream packet(&Body,QIODevice::WriteOnly);
            QByteArray name(body.UserName.toUtf8().data());
            QByteArray md5(body.FileMD5.toUtf8().data());
            packet<<name.size();
            packet<<md5.size();
            Body +=  name;
            Body += md5;
            Body += body.FileData;
            Header.Method = method;
            Header.MethodType = methodType;
            Header.PackSize =HeaderSize +   Body.size();
        }
    }
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
        m_buffer += Body;
        qDebug()<<"m_buffer size = "<<m_buffer.size()<<"Body size = "<<Body.size();
        return m_buffer;
    }
};

#endif // PACKET_DEFINE_H
