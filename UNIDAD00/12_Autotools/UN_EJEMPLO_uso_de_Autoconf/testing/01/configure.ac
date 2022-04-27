AC_INIT

AC_DEFUN(
  [CW_SYS_GREETING],
  [
    AC_CACHE_CHECK(
      [for greeting message],
      [cw_cv_system_greeting],
      [dnl
        AC_LANG_PUSH(C)
        AC_RUN_IFELSE(
          [dnl
            AC_LANG_PROGRAM([#include <stdio.h>], [fprintf(stderr,"hello \"world\"\n")]) 
          ],
          [cw_cv_system_greeting=$(./conftest$EXEEXT 2>&1 | sed 's/.*"\(.*\)".*/\1/')],
          [AC_MSG_ERROR(Failed to compile a test program!?)]
        )
        AC_LANG_POP(C) 
      ]
    )
    eval "CW_GREETING=$cw_cv_system_greeting"
    AC_SUBST(CW_GREETING)
    m4_pattern_allow(CW_GREETING)
    AC_DEFINE_UNQUOTED([CW_GREETING], $cw_cv_system_greeting, [The greeting to use.])
  ] 
)

CW_SYS_GREETING
AC_OUTPUT
