#ifndef PACKETPROCESS_H
#define PACKETPROCESS_H

#include "packet_define.h"

class PacketProcess
{
public:
    PacketProcess();
    void parsePack(QByteArray arr ,int packSize, int method,int methodType,void *,int packType);
};

#endif // PACKETPROCESS_H
