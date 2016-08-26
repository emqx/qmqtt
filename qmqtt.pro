TEMPLATE = subdirs

SUBDIRS = src examples

examples.depends = src

unix:!NO_UNIT_TESTS: {
    SUBDIRS += gtest tests
    tests.depends = gtest src
}
