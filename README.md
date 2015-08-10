QMQTT
=====

mqtt client for Qt

Usage
=====

	#include "qmqtt.h"

	QMQTT::Client *client = new QMQTT::Client("localhost", 1883);

	client->setClientId("clientId");

	client->setUsername("user");

	client->setPassword("password");

	client->connect();


Slots
=====

	void connect();

	quint16 publish(Message &message);

	void puback(quint8 type, quint16 msgid);

	quint16 subscribe(const QString &topic, quint8 qos);

	void unsubscribe(const QString &topic);

	void ping();

	void disconnect();

Signals
=======

	void connected();

	void error(QAbstractSocket::SocketError);

	void connacked(quint8 ack);

	void published(Message &message);

	void pubacked(quint8 type, quint16 msgid);

	void received(const Message &message);

	void subscribed(const QString &topic);

	void subacked(quint16 mid, quint8 qos);

	void unsubscribed(const QString &topic);

	void unsubacked(quint16 mid);

	void pong();

	void disconnected();


License
=======

The MIT License (MIT)


Contributors
=============

[@Kampfgnom](https://github.com/Kampfgnom)

[@rafaeldelucena](https://github.com/rafaeldelucena)

Author
======
Feng Lee <feng@emqtt.io>

