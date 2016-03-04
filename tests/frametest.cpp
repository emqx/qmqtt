#include <qmqtt_frame.h>
#include <QScopedPointer>
#include <QBuffer>
#include <QDataStream>
#include <gtest/gtest.h>

using namespace testing;

class FrameTest : public Test
{
public:
    explicit FrameTest()
        : _frame(new QMQTT::Frame)
    {
    }
    virtual ~FrameTest() {}

    QScopedPointer<QMQTT::Frame> _frame;
};

TEST_F(FrameTest, defaultConstructor_Test)
{
    EXPECT_EQ(0, _frame->_header);
    EXPECT_TRUE(_frame->_data.isEmpty());
}

TEST_F(FrameTest, constructorWithOneParameter_Test)
{
    _frame.reset(new QMQTT::Frame(42));

    EXPECT_EQ(42, _frame->_header);
    EXPECT_TRUE(_frame->_data.isEmpty());
}

TEST_F(FrameTest, constructorWithTwoParameters_Test)
{
    _frame.reset(new QMQTT::Frame(42, QString("abc").toUtf8()));

    EXPECT_EQ(42, _frame->_header);
    EXPECT_EQ(QString("abc").toUtf8(), _frame->_data);
}

TEST_F(FrameTest, copyConstructor_Test)
{
    _frame.reset(new QMQTT::Frame(42, QString("abc").toUtf8()));
    QMQTT::Frame frame(*_frame);

    EXPECT_EQ(42, frame._header);
    EXPECT_EQ(QString("abc").toUtf8(), frame._data);
}

TEST_F(FrameTest, assignmentOperator_Test)
{
    _frame.reset(new QMQTT::Frame(42, QString("abc").toUtf8()));
    QMQTT::Frame frame;
    frame = *_frame;

    EXPECT_EQ(42, frame._header);
    EXPECT_EQ(QString("abc").toUtf8(), frame._data);
}

TEST_F(FrameTest, equalityOperatorTrueIfHeaderAndDataSame_Test)
{
    _frame.reset(new QMQTT::Frame(42, QString("abc").toUtf8()));
    QMQTT::Frame frame(42, QString("abc").toUtf8());

    EXPECT_TRUE(frame == *_frame);
}

TEST_F(FrameTest, equalityOperatorFalseIfHeaderDifferent_Test)
{
    _frame.reset(new QMQTT::Frame(0, QString("abc").toUtf8()));
    QMQTT::Frame frame(42, QString("abc").toUtf8());

    EXPECT_FALSE(frame == *_frame);
}

TEST_F(FrameTest, equalityOperatorFalseIfDataDifferent_Test)
{
    _frame.reset(new QMQTT::Frame(42, QString("abc").toUtf8()));
    QMQTT::Frame frame(42, QString("def").toUtf8());

    EXPECT_FALSE(frame == *_frame);
}

TEST_F(FrameTest, inequalityOperatorTrueIfHeaderAndDataSame_Test)
{
    _frame.reset(new QMQTT::Frame(42, QString("abc").toUtf8()));
    QMQTT::Frame frame(42, QString("abc").toUtf8());

    EXPECT_FALSE(frame != *_frame);
}

TEST_F(FrameTest, inequalityOperatorFalseIfHeaderDifferent_Test)
{
    _frame.reset(new QMQTT::Frame(0, QString("abc").toUtf8()));
    QMQTT::Frame frame(42, QString("abc").toUtf8());

    EXPECT_TRUE(frame != *_frame);
}

TEST_F(FrameTest, inequalityOperatorFalseIfDataDifferent_Test)
{
    _frame.reset(new QMQTT::Frame(42, QString("abc").toUtf8()));
    QMQTT::Frame frame(42, QString("def").toUtf8());

    EXPECT_TRUE(frame != *_frame);
}

TEST_F(FrameTest, writeRemainingLengthOneByte_Test)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    QMQTT::Frame::writeRemainingLength(stream, 42);
    buffer.close();

    EXPECT_EQ(1, buffer.data().size());
    EXPECT_EQ(42, static_cast<quint8>(buffer.data().at(0)));
}

TEST_F(FrameTest, writeRemainingLengthTwoBytes_Test)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    QMQTT::Frame::writeRemainingLength(stream, 128);
    buffer.close();

    EXPECT_EQ(2, buffer.data().size());
    EXPECT_EQ(0x80, static_cast<quint8>(buffer.data().at(0)));
    EXPECT_EQ(0x01, static_cast<quint8>(buffer.data().at(1)));
}

TEST_F(FrameTest, readRemainingLengthOneByte_Test)
{
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);
    stream << static_cast<quint8>(42);
    buffer.seek(0);
    int remainingLength = QMQTT::Frame::readRemainingLength(stream);
    buffer.close();

    EXPECT_EQ(42, remainingLength);
}

TEST_F(FrameTest, readRemainingLengthTwoByte_Test)
{
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);
    stream << static_cast<quint8>(0x80);
    stream << static_cast<quint8>(0x01);
    buffer.seek(0);
    int remainingLength = QMQTT::Frame::readRemainingLength(stream);
    buffer.close();

    EXPECT_EQ(128, remainingLength);
}

TEST_F(FrameTest, writeToStream_Test)
{
    _frame.reset(new QMQTT::Frame(42, QByteArray(128, 'a')));

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << *_frame;
    buffer.close();

    EXPECT_EQ(131, buffer.data().size());
    EXPECT_EQ(42, static_cast<quint8>(buffer.data().at(0)));
    EXPECT_EQ(0x80, static_cast<quint8>(buffer.data().at(1)));
    EXPECT_EQ(0x01, static_cast<quint8>(buffer.data().at(2)));
    EXPECT_EQ(QByteArray(128, 'a'), buffer.data().right(128));
}

TEST_F(FrameTest, readFromStream_Test)
{
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);
    stream << static_cast<quint8>(42);
    stream << static_cast<quint8>(0x80);
    stream << static_cast<quint8>(0x01);
    QByteArray byteArray(128, 'a');
    stream.writeRawData(byteArray.constData(), byteArray.size());
    buffer.seek(0);
    stream >> *_frame;
    buffer.close();

    EXPECT_EQ(42, _frame->_header);
    EXPECT_EQ(128, _frame->_data.size());
    EXPECT_EQ(byteArray, _frame->_data);
}
