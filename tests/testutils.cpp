#include "testutils.h"

void TestUtils::writeString(QDataStream& stream, const QString& string)
{
    QByteArray byteArray = string.toUtf8();
    stream << static_cast<quint16>(byteArray.size());
    stream.writeRawData(byteArray.constData(), byteArray.size());
}
