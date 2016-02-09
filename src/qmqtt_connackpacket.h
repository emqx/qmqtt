#ifndef QMQTT_CONNACK_PACKET_H
#define QMQTT_CONNACK_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>
#include <qglobal.h>

namespace QMQTT
{

class ConnackPacket : public AbstractPacket
{
public:
    ConnackPacket();
    virtual ~ConnackPacket();

    enum ConnectReturnCodeType
    {
        ConnectionAccepted = 0x00,
        ConnectionRefusedUnacceptableProtocolVersion = 0x01,
        ConnectionRefusedIdentifierRejected = 0x02,
        ConnectionRefusedServerUnavailable = 0x03,
        ConnectionRefusedBadUserNameOrPassword = 0x04,
        ConnectionRefusedNotAuthorized = 0x05
    };

    bool sessionPresent() const;
    ConnectReturnCodeType connectReturnCode() const;

    void setSessionPresent(const bool sessionPresent);
    void setConnectReturnCode(const ConnectReturnCodeType connectReturnCode);

    bool isValid() const;
    PacketType type() const;
    qint64 calculateRemainingLengthFromData() const;

protected:
    quint8 _connectAcknowledgeFlags;
    ConnectReturnCodeType _connectReturnCode;

private:
    friend QDataStream& operator>>(QDataStream& stream, ConnackPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const ConnackPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, ConnackPacket& packet);
QDataStream& operator<<(QDataStream& stream, const ConnackPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_CONNACK_PACKET_H
