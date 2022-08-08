#include <stdlib.h>
#include <ctype.h>

char *strcasestr(const char *str, const char *pattern) {
    size_t i;

    if (!*pattern)
        return (char*)str;

    for (; *str; str++) {
        if (toupper(*str) == toupper(*pattern)) {
            for (i = 1;; i++) {
                if (!pattern[i])
                    return (char*)str;
                if (toupper(str[i]) != toupper(pattern[i]))
                    break;
            }
        }
    }
    return NULL;
}


