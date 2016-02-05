TEMPLATE = subdirs

SUBDIRS = src example
src.file = src/qmqtt.pro

example.depends = src

unix:!NO_UNIT_TESTS: {
    SUBDIRS += gtest tests
    tests.depends = gtest src

    check.target = check
    check.CONFIG = recursive
    QMAKE_EXTRA_TARGETS += check
}
