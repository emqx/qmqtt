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

#endif // FRAME_TESTS_H
