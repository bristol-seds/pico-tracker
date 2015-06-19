# Recursive Make
SUBDIRS := $(wildcard */Makefile)

all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $(@D)

.PHONY: all $(SUBDIRS)
