TEMPLATE = subdirs
SUBDIRS += \
    auto

unix:!NO_UNIT_TESTS: SUBDIRS += gtest
