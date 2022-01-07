#include "packet_process.h"
#include <QDebug>
PacketProcess::PacketProcess()
{

}

LoginBody PacketProcess::parseLoginPack(QByteArray arr)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(arr, &jsonError));
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << jsonError.errorString();
        return LoginBody();
    }
    auto values = jsonDoc.object();
    LoginBody loginBody;
    loginBody.UserName = values.value("UserName").toString();
    loginBody.PassWord = values.value("PassWord").toString();
    loginBody.Notice = values.value("Notice").toBool();
    loginBody.MacAddress = values.value("MacAddress").toString();
    loginBody.LoginTime = values.value("LoginTime").toString();
    loginBody.LoginStatus = values.value("LoginStatus").toInt();
    return loginBody;
}

MsgBody PacketProcess::parseMsgPack(QByteArray arr)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(arr, &jsonError));
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << jsonError.errorString();
        return MsgBody();
    }
    auto values = jsonDoc.object();
    MsgBody  msgBody;
    msgBody.UserName    = values.value("UserName").toString();
    msgBody.Msg         = values.value("Msg").toString();
    msgBody.MsgType     = values.value("MsgType").toInt();
    msgBody.DstUser     = values.value("DstUser").toString();
    msgBody.DstUserID   = values.value("DstUserID").toInt();
    return msgBody;
}

OnlineListBody PacketProcess::parseOnlineListBodyPack(QByteArray arr)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(arr, &jsonError));
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << jsonError.errorString();
        return OnlineListBody();
    }
    auto values = jsonDoc.object();
    OnlineListBody  onlineListBody;
    onlineListBody.UserName     = values.value("UserName").toString();
    onlineListBody.Status       = values.value("Status").toInt();
    return onlineListBody;
}

FileBody PacketProcess::parseFileDataPack(QByteArray arr)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(arr, &jsonError));
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << jsonError.errorString();
        return FileBody();
    }
    auto values = jsonDoc.object();
    FileBody  fileBody;
    fileBody.UserName       = values.value("UserName").toString();
    fileBody.FileName       = values.value("FileName").toString();
    fileBody.FileMD5        = values.value("FileMD5").toString();
    fileBody.TotalSize      = values.value("TotalSize").toInt();
    fileBody.CurrentSize    = values.value("CurrentSize").toInt();
    fileBody.DstUserName    = values.value("DstUserName").toString();
    fileBody.SendStatus     = values.value("SendStatus").toInt();
    fileBody.FileData       = (char*)values.value("FileData").toVariant().data();
    return fileBody;
}
