## Verification

#### Setup

```
sudo apt-get install python-pip
sudo pip install colorama
```

#### Usage

Something like this.

`> /dev/null arm-none-eabi-gdb -q -x tools/verification/list/protocol.py `

You need to have your debugger configured in config.mk or I imagine
things won't work to well

#### Operation

Initially the `verification.py` loads the latest binary, and runs
`Reset_Handler` until the top of `main`. It then jumps to `tc_main` instead.

While stopped in `tc_main` a pointer to the test case is set. The
program is then run, and one loop of `tc_main` runs the test case.

#### Writing a new test case

Several naming conventions need to the followed for `verification.py`
to find everything

.
.
.
