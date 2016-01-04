TEMPLATE = subdirs

SUBDIRS = src example
src.file = src/qmqtt.pro

unix {
    SUBDIRS += gtest tests
    tests.depends = gtest src
}
