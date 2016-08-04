# Configuration makefile
# Copyright (C) 2014  Richard Meadows <richardeoin>
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Project Name
#
# This is used to define the name of the build artifact
#
PROJECT_NAME		:= pico

# The exact chip being built for.
#
TARGET_CHIP		:= SAMD20E18

# Compiliation Flags
#
# Use this to set the debug level
#
COMPILATION_FLAGS	:= -g3 -ggdb

# Acceptable Warnings
#
ACCEPT_WARN		:=

# Linker Flags
#
LINKER_FLAGS		:= -Wl,--gc-sections

# The path to a specific blackmagic debugger to use. Optional
#
# You can use `make print-symlinks DEVICE=<debugger name>` to find a
# path to the debugger that will be constant for a given device or
# port. When this field is specified GDB will attempt to connect to
# this debugger on startup.
#
BLACKMAGIC_PATH		:=
