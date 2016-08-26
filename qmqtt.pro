TEMPLATE = subdirs

SUBDIRS = src example

example.depends = src

unix:!NO_UNIT_TESTS: {
    SUBDIRS += gtest tests
    tests.depends = gtest src
}
