#ifndef __ERRORES__
#define __ERORRES__
#include <stdio.h>

#define error_fatal(codigo, texto) do {\
    fprintf (stderr, "%s:%d: ERROR: %s - %s\n",\
        __FILE__, __LINE__, texto, strerror (codigo));\
    abort ();\
    } while (0)

#endif /*__ERRORES__*/


