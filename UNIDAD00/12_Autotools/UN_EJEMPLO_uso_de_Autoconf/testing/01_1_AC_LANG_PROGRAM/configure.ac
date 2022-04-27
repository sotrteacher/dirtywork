dnl https://www.gnu.org/software/autoconf/manual/autoconf-2.63/html_node/Generating-Sources.html
     AC_INIT([Hello], [1.0], [bug-hello@example.org])
     AC_DEFINE([HELLO_WORLD], ["Hello, World\n"],
       [Greetings string.])
     AC_LANG_CONFTEST(
     [AC_LANG_PROGRAM([[const char hw[] = "Hello, World\n";]],
                      [[fputs (hw, stdout);]])])
     gcc -E -dD -o - conftest.c
     
