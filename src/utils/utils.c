#include <utils/utils.h>

int memidentical(const void * b1, const void * b2, size_t n) {

    /* Iterate over all n bytes even if the result can be determined earlier */

    int result = 0;
    const u8 * p1 = b1;
    const u8 * p2 = b2;

    while (n -- > 0) {

        result |= (*p1 ^ *p2);
        p1++;
        p2++;
    }

    return 1 - result;
}

void purge_secrets(void * secrets, size_t size) {

    /* TODO: Use memset_s when available (when using C11) */
    volatile u8 * p = secrets;
    while (size -- > 0) {
        *p++ = 0;
    }
}
