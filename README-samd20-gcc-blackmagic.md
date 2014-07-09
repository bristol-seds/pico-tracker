## samd20-gcc-blackmagic ##

A simple GCC setup for Atmel SAM D20 development, intended for use with the
[blackmagic debug probe](https://github.com/gsmcmullin/blackmagic).

## Prerequisites ##

[GNU Make](http://www.gnu.org/software/make/) and the following
standard utilities are required: `cat`, `echo`, `find`, `grep`,
`mkdir`, `rm`, `sed` and `tr`. If you're running any sensible desktop
linux then these will already be installed.

You will also need to aquire
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded/).

##### On Ubuntu

```
sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded
sudo apt-get update && sudo apt-get install gcc-arm-none-eabi
```

## Usage ##

### Project Options ###

[`config.mk`](config.mk) allows configuation for individual
projects. In particular the `PROJECT_NAME` and `TARGET_CHIP` variables
needs to be set.

### Compiling ###

`make`

## Backmagic ##

### Download ###

Run `arm-none-eabi-gdb`. If you have set `BLACKMAGIC_PATH` in
[`config.mk`](config.mk) then gdb will attempt to connect to the
blackmagic debugger. Otherwise you can use the `blackmagic` command to
connect a `/dev/ttyACM<n>` device. For example `blackmagic 0` will
connect to a blackmagic at `/dev/ttyACM0`.

To attach to the SAM D20 chip itself you will need to run something like

```
monitor swdp_scan
attach 1
```

You can place these commands in a `gdbscript-custom` file so that in
future they will be run automatically. If `monitor swdp_scan` fails to
detect an attached SAM D20 device then you may need to upgrade the
firmware on your blackmagic to the latest version.

To download code run

```
monitor erase_mass
load
```

### Debugging ###

You can start with the gdb command

```
run
```

You might want to use the command `set confirm off` so that you're not
prompted each time. You might also want to `set mem
inaccessible-by-default off` so that you can look at memory locations
outside of RAM and ROM.

These commands can be automated by placing them in a `gdbscript-custom` file.

### Semihosting ###

You can build for semihosting by defining the `SEMIHOSTING` variable
in make. Like this:

```
make -kB SEMIHOSTING=1
```

This build will be significantly larger (in terms of RAM and ROM).

For this build, when a debugger is present at startup the
`semihost_printf`, `semihost_puts` and `semihost_putchar` functions
will print to stdio on the host. The Blackmagic Debug Probe supports
this.

## Emacs ##

The command `make emacs` can be used to quickly launch an instance of
emacs with all the relavent source code loaded.

A `TAGS` file can be generated with `make all` or `make tags`, and
this can be used to automat navigate the source code. See the
[emacs manual](https://www.gnu.org/software/emacs/manual/html_node/emacs/Tags.html)
for more details.

A Directory Local Variables File [`.dir-locals.el`](.dir-locals.el)
exists in the root of the project. This has the following effects
on emacs:

### Fixed default-directory ###

The default directory is fixed as the root of the project wherever you
are within the project. As the makefile needs to be run from the root
of the project, this means that `M+x compile` will always run the
top-level makefile no matter which file you are editing.

### Custom GDB ###

`M+x gdb` is set to use `arm-none-eabi-gdb` rather than the default GDB for your
machine.

## Other notes ##

Wherever possible use
[Function Attributes](http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html)
and
[Variable Attrubutes](http://gcc.gnu.org/onlinedocs/gcc/Variable-Attributes.html)
rather than editing the project's linker files.

## Sources & Licensing ##

See [LICENSE.md](LICENSE-samd20-gcc-blackmagic.md)
