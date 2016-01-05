TEMPLATE = subdirs

SUBDIRS = src example
src.file = src/qmqtt.pro

example.depends = src

unix {
    SUBDIRS += gtest tests
    tests.depends = gtest src
}
