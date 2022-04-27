AC_INIT([Hello], [1.0], [bug-hello@example.org])
AC_DEFINE([HELLO_WORLD], ["Hello, World\n"],
[Greetings string.])
AC_LANG(C)
AC_LANG_CONFTEST(
        [AC_LANG_SOURCE([[const char hw[] = "Hello, World\n";]])])
gcc -E -dD -o - conftest.c

AC_OUTPUT

