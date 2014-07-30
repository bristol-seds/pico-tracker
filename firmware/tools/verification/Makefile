# Makefile for verification tests

all: verification_tc.py

%.py: %.c
	@echo "Generating Python Wrappers...."
	@echo
	ctypesgen/ctypesgen.py -o $@ --cpp="gcc -E -DCTYPESGEN" $<
