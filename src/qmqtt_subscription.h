#ifndef QMQTT_SUBSCRIPTION_H
#define QMQTT_SUBSCRIPTION_H

#include <QString>

namespace QMQTT
{

class Subscription
{
public:
    QString _topicFilter;
    quint8 _requestedQos;
};

typedef QList<Subscription> SubscriptionList;

} // end namespace QMQTT

#endif // QMQTT_SUBSCRIPTION_H

