/*
 * Wrapper functions for semihosting
 * Copyright (C) 2014  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SEMIHOSTING_H
#define SEMIHOSTING_H

/**
 * Please use the semihost_*() wrappers so that semihosting functions
 * can be disables from the makefile!
 */

#ifndef __SEMIHOSTING__

/* Dummy function handlers */
#define semihost_putchar(c)	(void)(c)
#define semihost_puts(s)	(void)(s)
#define semihost_printf(...)    __dummy_printf(__VA_ARGS__)

void __dummy_printf(const char *format, ...);

#else

/* We would get a SVC exception if we called stdio without a debug probe */
void set_semihosting(void);

/* Real function handlers */
#define semihost_putchar(c)	__putchar(c)
#define semihost_puts(s)	__puts(s)
#define semihost_printf(...)	__printf(__VA_ARGS__)

void __putchar(char c);
void __puts(const char* s);
void __printf(const char *format, ...);



#endif /* __SEMIHOSTING__ */

#endif /* SEMIHOSTING_H */
