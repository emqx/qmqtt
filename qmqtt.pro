TEMPLATE = subdirs

SUBDIRS = src examples

examples.depends = src

SUBDIRS += tests
tests.depends = src
