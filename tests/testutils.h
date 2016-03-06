#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QDataStream>
#include <QString>
#include <QByteArray>

namespace TestUtils
{
    void writeString(QDataStream& stream, const QString& string);
}

#endif // TESTUTILS_H
