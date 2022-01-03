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
    msgBody.User = values.value("User").toString();
    msgBody.UserID = values.value("UserID").toInt();
    msgBody.MsgType = values.value("MsgType").toInt();
    msgBody.UserMsg = values.value("UserMsg").toString();
    return msgBody;
}
