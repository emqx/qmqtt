OTHER_FILES += \
    $${TEST_FILES} \
    tests/tests.pri

unix {
    unit_tests_directory = tests
    unit_tests.target = all
    unit_tests.commands = \
        mkdir -p $${OUT_PWD}/$${unit_tests_directory}; \
        cd $${OUT_PWD}/$${unit_tests_directory}; \
        for pro_path in $$find(TEST_FILES,\.pro$); do \
            pro_filename=\$\${pro_path$${LITERAL_HASH}$${LITERAL_HASH}*/}; \
            pro_basename=\$\${pro_filename%.pro}; \
            $${QMAKE_QMAKE} -o Makefile.\$\${pro_basename} $${PWD}/\$\${pro_filename}; \
            LD_LIBRARY_PATH=$${OUT_PWD} make -f Makefile.\$\${pro_basename} check; \
        done; \
        cd $${OUT_PWD}
    QMAKE_EXTRA_TARGETS += unit_tests
}
