#ifndef QMQTT_MESSAGE_P_H
#define QMQTT_MESSAGE_P_H

#include <QString>
#include <QByteArray>

namespace QMQTT {

class MessagePrivate
{
public:
    explicit MessagePrivate();
    virtual ~MessagePrivate();

    void init(quint16 id, const QString &topic, const QByteArray &payload,
              quint8 qos, bool retain, bool dup);

    MessagePrivate(const MessagePrivate& other);
    MessagePrivate& operator=(const MessagePrivate& other);
    bool operator==(const MessagePrivate& other) const;

    quint16 id() const;
    void setId(const quint16 id);
    quint8 qos() const;
    void setQos(const quint8 qos);
    bool retain() const;
    void setRetain(const bool retain);
    bool dup() const;
    void setDup(const bool dup);
    QString topic() const;
    void setTopic(const QString& topic);
    QByteArray payload() const;
    void setPayload(const QByteArray& payload);

    quint16 _id;
    QString _topic;
    QByteArray _payload;
    quint8 _qos;
    bool _retain;
    bool _dup;
};

} // namespace QMQTT

#endif // QMQTT_MESSAGE_P_H
