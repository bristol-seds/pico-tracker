## Verification

#### Setup

```
sudo apt-get install python-pip
sudo pip install colorama
```

#### Usage

Something like this.

`> /dev/null arm-none-eabi-gdb -q -x tools/tests.py `

You need to have your debugger configured in config.mk and possibly
also have gdbscript-custom. The test driver just issues an `attach 1`
command after gdb startup and expects that to work.

##### From the makefile

From the main firmware makefile you can just run

```
make test
```

to run all tests, or

```
make test tc=<tc name>
```

to run a specific test case. To get extra debug info

```
make test tc=<tc name> tc-gdb-info=1
```

#### Operation

Initially `tests.py` loads the latest binary, and runs `Reset_Handler`
until the top of `main`. It then jumps to `tc_main` instead.

While stopped in `tc_main` a pointer to the test case is set. The
program is then run, and one loop of `tc_main` runs the test case.

#### Writing a new test case

```
From this directory you can just run 'make name=<new_name>'
```

* Choose a testcase name `[tc-name]`
* Create `tc/[tc-name].py` and `tc/[tc_name].h`. Use a pre-existing test case as a template.
* Add `#include [tc-name].h` to the section at the top of `tmain.c`

You'll need to fill in the `/* Parameters In */` and `/* Results Out
*/` structures in `tc/[tc-name].h`

`tc/[tc-name].py` must contain a class called `[tc-name]_tc` that
defines `get_test` and `is_correct` methods.
