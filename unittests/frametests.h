#ifndef FRAME_TESTS_H
#define FRAME_TESTS_H

#include <QObject>
#include <QScopedPointer>

namespace QMQTT
{
    class Frame;
}

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

    void default_constructor();
    void constructor_with_parameters();

    void readInt_reads_bytearray();
    void readChar_reads_bytearray();
    void readString_reads_bytearray();

    void writeInt_writes_to_bytearray();
    void writeChar_writes_to_bytearray();
    void writeString_writes_to_bytearray();
    void writeRawData_writes_to_bytearray();
    void write_writes_to_datastream();
};

#endif // FRAME_TESTS_H
