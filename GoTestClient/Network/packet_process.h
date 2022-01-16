#ifndef PACKETPROCESS_H
#define PACKETPROCESS_H

#include "packet_define.h"

class PacketProcess
{
public:
    PacketProcess();

    LoginBody       parseLoginPack(QByteArray arr);

    MsgBody         parseMsgPack(QByteArray arr);

    OnlineListBody  parseOnlineListBodyPack(QByteArray arr);

    FileBody        parseFileDataPack(QByteArray arr);


};

#endif // PACKETPROCESS_H
