TEMPLATE = subdirs
SUBDIRS =

unix:!NO_UNIT_TESTS: SUBDIRS += gtest
