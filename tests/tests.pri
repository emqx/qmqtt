TEST_SOURCES += \
#    testmain.cpp \
#    willtests.cpp \
#    messagetests.cpp \
#    frametests.cpp \
    networktests.cpp \
    tcpserver.cpp \
#    routedmessagetests.cpp \
#    routertests.cpp \
#    routesubscriptiontests.cpp \
    clienttests.cpp

TEST_HEADERS += \
#    willtests.h \
#    messagetests.h \
#    frametests.h \
#    networktests.h \
    tcpserver.h \
#    routedmessagetests.h \
#    routertests.h \
#    routesubscriptiontests.h

for(file, TEST_SOURCES) {
     TEST_FILES += tests/$${file}
}

for(file, TEST_HEADERS) {
     TEST_FILES += tests/$${file}
}

TEST_FILES += \
#    tests/tests.pro \
    tests/test.pri

OTHER_FILES += $${TEST_FILES}

