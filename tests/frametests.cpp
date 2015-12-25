#include <qmqtt_frame.h>
#include <QTest>
#include <QScopedPointer>

class FrameTests : public QObject
{
    Q_OBJECT
public:
    explicit FrameTests();
    virtual ~FrameTests();

    QScopedPointer<QMQTT::Frame> _uut;

private slots:
    void init();
    void cleanup();

    void defaultConstructor_Test();
    void constructorWithParameters_Test();

    // todo: copy constructor
    // todo: assignment operator
    // todo: equality operator

    void readIntReadsBytearray_Test();
    void readCharReadsBytearray_Test();
    void readStringReadsBytearray_Test();

    void writeIntWritesToBytearray_Test();
    void writeCharWritesToBytearray_Test();
    void writeStringWritesToBytearray_Test();
    void writeRawDataWritesToBytearray_Test();
    void writeWritesToDatastream_Test();
};

FrameTests::FrameTests()
    : _uut(NULL)
{
}

FrameTests::~FrameTests()
{
}

void FrameTests::init()
{
    _uut.reset(new QMQTT::Frame(0));
}

void FrameTests::cleanup()
{
    _uut.reset();
}

void FrameTests::defaultConstructor_Test()
{
    QSKIP("Disabled test, getters segfault");
    // todo: these segfault currently. QByteArray contains no data when default constructed.
    // QCOMPARE(_uut->readInt(), 0);
    // QCOMPARE(_uut->readChar(), static_cast<char>(12));
    // QCOMPARE(_uut->readString(), QString("blah"));
}

void FrameTests::constructorWithParameters_Test()
{
    QByteArray byteArray;
    _uut.reset(new QMQTT::Frame(0, byteArray));

    QCOMPARE(_uut.isNull(), false);
}

void FrameTests::readIntReadsBytearray_Test()
{
    QByteArray byteArray;
    byteArray.resize(2);
    byteArray[0] = 1;
    byteArray[1] = 2;
    _uut.reset(new QMQTT::Frame(0, byteArray));

    QCOMPARE(_uut->readInt(), static_cast<int>((1 << 8) + 2));
}

void FrameTests::readCharReadsBytearray_Test()
{
    QByteArray byteArray;
    byteArray.resize(1);
    byteArray[0] = 42;
    _uut.reset(new QMQTT::Frame(0, byteArray));

    QCOMPARE(_uut->readChar(), static_cast<char>(42));
}

void FrameTests::readStringReadsBytearray_Test()
{
    QByteArray byteArray;
    byteArray.resize(5);
    byteArray[0] = 0;
    byteArray[1] = 3;
    byteArray[2] = 'a';
    byteArray[3] = 'b';
    byteArray[4] = 'c';
    _uut.reset(new QMQTT::Frame(0, byteArray));

    QCOMPARE(_uut->readString(), QString("abc"));
}

void FrameTests::writeIntWritesToBytearray_Test()
{
    _uut->writeInt(42);

    QCOMPARE(_uut->readInt(), 42);
}

void FrameTests::writeCharWritesToBytearray_Test()
{
    _uut->writeChar(static_cast<char>(42));

    QCOMPARE(_uut->readChar(), static_cast<char>(42));
}

void FrameTests::writeStringWritesToBytearray_Test()
{
    _uut->writeString(QString("forty-two"));

    QCOMPARE(_uut->readString(), QString("forty-two"));
}

void FrameTests::writeRawDataWritesToBytearray_Test()
{
    QByteArray byteArray;
    byteArray.resize(2);
    byteArray[0] = 3;
    byteArray[1] = 4;
    _uut->writeRawData(byteArray);

    QCOMPARE(_uut->readInt(), static_cast<int>((3 << 8) + 4));
}

void FrameTests::writeWritesToDatastream_Test()
{
    QByteArray frameData(128, 0);
    for (unsigned char c = 0; c < 128; c++)
    {
        frameData[c] = c;
    }
    _uut.reset(new QMQTT::Frame(0, frameData));

    QByteArray streamedData;
    QDataStream stream(&streamedData, QIODevice::WriteOnly);
    _uut->write(stream);

    QCOMPARE(static_cast<quint8>(streamedData.at(0)), static_cast<quint8>(0));
    QCOMPARE(static_cast<quint8>(streamedData.at(1)), static_cast<quint8>(0x80));
    QCOMPARE(static_cast<quint8>(streamedData.at(2)), static_cast<quint8>(0x01));
    QCOMPARE(streamedData.mid(3), frameData);
}

QTEST_MAIN(FrameTests);
#include "frametests.moc"
