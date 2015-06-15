# Compiles firmware written in C and assembler for the Atmel SAM D20
# Copyright (C) 2014
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

#
# The primary targets in this file are:
#
# all				Everything
# print-symlinks DEVICE=<dev>	Prints any symlinks to a given device
# etags				Generates an ETAGS file for the project
# emacs				Launches emacs for this project
# verification			Runs the ctypesgen needs for verif. scripts
# clean				Removes generated files
#
# This makefile is intended to be run from the root of the project.
#

# External configuration makefile
#
# Edit the project name, and compilation flags etc. in this makefile
#
-include config.mk

# Directories
#
# These define the locations of the source, nordic sdk and output trees.
#
OUTPUT_PATH	:= out/
SOURCE_TREE	:= src/
INCLUDE_PATH	:= inc/

# Shell Commands
#
# Listed here for portability.
#
CAT		:= cat
ECHO		:= echo
FIND		:= find
GREP		:= grep
MKDIR		:= mkdir -p
RM		:= rm -r
SED		:= sed
TR		:= tr

# ARM GCC Toolchain
#
# These tools are available from https://launchpad.net/gcc-arm-embedded/ and
# should be placed on your path. ALternatively you could compile your own.
#
TOOLCHAIN  	:= arm-none-eabi
AS		:= $(TOOLCHAIN)-as
CC		:= $(TOOLCHAIN)-gcc
DB		:= $(TOOLCHAIN)-gdb
CXX		:= $(TOOLCHAIN)-g++
OBJCOPY		:= $(TOOLCHAIN)-objcopy
OBJDUMP		:= $(TOOLCHAIN)-objdump
SIZE		:= $(TOOLCHAIN)-size

# The SAM D20 series is based on an ARM Cortex M0+ core
#
#
ARCH_FLAGS 	:= -mcpu=cortex-m0plus -mthumb

# Flags for float printing
#
#
LDFLAGS	+= --specs=rdimon.specs -lrdimon

# Compilation Flags
#
# Display all warnings. Compile functions and data into their own sections so
# they can be discarded if unused.  The linker performs garbage collection of
# unused input sections.
#
CFLAGS		+= $(COMPILATION_FLAGS) -Wall -Wextra $(ACCEPT_WARN) -std=gnu99 \
			-ffunction-sections -fdata-sections $(ARCH_FLAGS)
ASFLAGS		+= -Wall $(ARCH_FLAGS) -a=/dev/null
LDFLAGS		+= $(COMPILATION_FLAGS) $(LINKER_FLAGS) -Wextra $(ARCH_FLAGS)

# Compilation Defines
#
# These are available for use as macros
#
ifdef TARGET_CHIP
CFLAGS		+= -D$(TARGET_CHIP) -D__$(TARGET_CHIP)__
endif
ifdef SEMIHOSTING
CFLAGS		+= -D__SEMIHOSTING__
endif

# Startup and system code
#
#
SYSTEM		?= chip/system_samd20.c chip/startup_samd20.c
INCLUDE_PATH	+= chip/ chip/cmsis/ samd20/ samd20/component/ test/tc/

# Verification suite code
#
#
SYSTEM		+= test/tmain.c

# Linker Scripts
#
#
LINKERS		?= chip/$(shell echo $(TARGET_CHIP) | $(TR) A-Z a-z).ld chip/sections.ld

# Our compilation target
#
#
TARGET		:= $(OUTPUT_PATH)$(PROJECT_NAME)

# Build our list of all our sources
#
# The entirety of the source directory are included, along with
# everything in certain directories in the SDK. This has security
# implications: Anything that makes it into your source tree will get
# compiled and linked into your binary.
#
VPATH		= $(SOURCE_PATH)
TREE_SOURCES	= $(shell $(FIND) $(SOURCE_TREE) -name '*.[csS]')
SOURCES 	= $(SYSTEM) $(TREE_SOURCES)

# Translate this list of sources into a list of required objects in
# the output directory.
objects		= $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(patsubst %.S,%.o, \
			$(SOURCES))))
OBJECTS		= $(addprefix $(OUTPUT_PATH),$(objects))

# Assemble a list of c and h files that are used in this project
#
TAGFILES	= $(SOURCES) $(shell $(CAT) $(OBJECTS:.o=.d) \
			| $(SED) -n '/^.*\.h:/p' | $(SED) 's/://g')

# Default target
#
#
all: $(TARGET).elf etags

# Rule for generating object and dependancy files from source files.
#
# Creates a directory in the output tree if nessesary. File is only
# compiled, not linked. Dependancy generation is automatic, but only
# for user header files. Every depandancy in the .d is appended to the
# .d as a target, so that if they stop existing the corresponding
# object file will be re-compiled.
#
$(OUTPUT_PATH)%.o: %.c
	@$(ECHO)
	@$(ECHO) 'Compiling $<...'
	@$(MKDIR) $(OUTPUT_PATH)$(dir $<)
	$(CC) -c -MMD -MP $(CPPFLAGS) $(CFLAGS) $(addprefix -I,$(INCLUDE_PATH)) -o $@ $<

# Attempt to include the dependancy makefiles for every object in this makefile.
#
# This means that object files depend on the header files they include.
#
-include $(OBJECTS:.o=.d)

# Rule for generating object files from assembler files
#
# Creates a directory in the output tree if nessesary. The file is only
# assembled, not linked.
#
$(OUTPUT_PATH)%.o: %.s
	@$(ECHO)
	@$(ECHO) 'Assembling $<...'
	@$(MKDIR) $(OUTPUT_PATH)$(dir $<)
	$(AS) $(ASFLAGS) -o $@ $<

# Generate the main build artifact.
#
# A .elf containing all the symbols (i.e. debugging information if the compiler
# / linker was run with -g) is created, alongside an intel hex file. A just
# about human-readable .map is also created.
#
$(TARGET).elf: $(OBJECTS) $(LINKERS) gdbscript Makefile config.mk
	@$(ECHO)
	@$(ECHO) 'Linking $@...'
	$(CC) $(LDFLAGS) $(addprefix -T,$(LINKERS)) -Wl,-Map,$(@:.elf=.map) -o $@ $(OBJECTS)
	@$(OBJCOPY) -O binary $@ $(@:.elf=.bin)
	@$(OBJCOPY) -O ihex $@ $(@:.elf=.hex)

	@$(ECHO)
	$(SIZE) $@
	@$(ECHO)
	@$(SIZE) $@ | tail -1 - \
		| awk '{print "ROM Usage: "int(($$1+$$2)/10.24)/100"K"}'
	@$(SIZE) $@ | tail -1 - \
		| awk '{print "RAM Usage: "int(($$2+$$3)/10.24)/100"K"}'

	@$(ECHO)

# Creates debugging command list for gdb
#
# These tell gdb which file to debug and which debugger to use
#
gdbscript: Makefile config.mk
	@$(ECHO) "# Load our .elf file into GDB" > gdbscript
	@$(ECHO) "file $(TARGET).elf" >> gdbscript
ifdef BLACKMAGIC_PATH
	@$(ECHO) "# Connect to a specified blackmagic" >> gdbscript
	@$(ECHO) "target extended-remote $(BLACKMAGIC_PATH)" >> gdbscript
endif

# Prints a list of symlinks to a device
#
# Use it like `make print-symlinks DEVICE=/dev/ttyACM0`
#
.PHONY: print-symlinks
print-symlinks:
	@$(ECHO) 'Symlinks to $(DEVICE):'
	@udevadm info --query symlink -n $(DEVICE) | \
		$(SED) -e 's/ /\n/' | $(SED) -e 's/^/\t/'

# Generates an etags file for the project
#
#
etags: $(TAGFILES)
	@$(ECHO) "Generating ETAGS..."
	@etags $^

# Launches emacs with all the files used for this project
#
.PHONY: emacs
emacs:
	@emacs23 $(TAGFILES) Makefile config.mk README.md

# Test
#
TESTCASES	:= $(shell $(FIND) test/tc -name '*.[h]')

.PHONY: test
test: test/main.py all
	@echo "Running tests..."
	$(DB) -q -x test/tests.py

# Ctypesgen for test
test/main.py: test/tmain.c $(TESTCASES)
	@echo "Generating Python Wrappers...."
	@echo
	test/ctypesgen/ctypesgen.py -o $@ \
	--cpp="$(CC) -E -DCTYPESGEN $(CPPFLAGS) $(CFLAGS) \
	$(addprefix -I,$(INCLUDE_PATH))" $^

# Removes everything in the output directory
#
#
#
.PHONY: clean
clean:
	$(RM) $(OUTPUT_PATH)*
	$(RM) gdbscript
	$(RM) TAGS
	$(RM) test/main.py*
