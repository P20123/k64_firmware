#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void *memset(void *dest, int c, size_t size) {
    unsigned int words = size >> 2; // divide by four
    unsigned int chars = size - words;
    for(int i = 0; i < words; i++) {
        ((uint32_t*)dest)[i] = (c << 24) | (c << 16) | (c << 8) | c;
    }
    for(int i = 0; i < chars; i++) {
        ((char*)dest + (words << 2))[i] = c;
    }
    return dest;
}

size_t strlen(const char *c) {
    size_t size = 0;
    if(c == NULL) goto done;
    for(;;) {
        // check for zeros but fetch 4 words at a time, decreasing memory
        // accesses.
        uint32_t fetch4 = *(uint32_t *)c;
        if(!(fetch4 & 0xff)) {
            goto done;
        }
        else if(!(fetch4 & 0xff00)) {
            size += 1;
            goto done;
        }
        else if(!(fetch4 & 0xff0000)) {
            size += 2;
            goto done;
        }
        else if(!(fetch4 & 0xff000000)) {
            size += 3;
            goto done;
        }
        else {
            size += 4;
            c += 4; // move ahead four bytes
        }
    }
done:
    return size;
}

void *memcpy(void *__restrict dest, const void *__restrict src, size_t size) {
    unsigned int words = size >> 2; // divide by four
    unsigned int chars = size - words;
    for(int i = 0; i < words; i++) {
        *((uint32_t*)dest + i) = *((uint32_t*)src + i);
    }
    for(int i = 0; i < chars; i++) {
        *((char*)dest + (words << 2) + i) = *((char*)src + (words << 2) + i);
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    unsigned int words = n >> 2; // divide by four
    unsigned int chars = n - words;
    int order = 0;
    int word_idx = 0;
    int byte_idx = 0;
    for(word_idx = 0; word_idx < words; word_idx++) {
        uint32_t a = *((uint32_t*)s1 + word_idx);
        uint32_t b = *((uint32_t*)s2 + word_idx);
        if(a < b) {
            order = -1;
            goto done;
        }
        else if(a > b) {
            order = 1;
            goto done;
        }
    }
    byte_idx = word_idx << 2; // multiply by four
    for(int i = byte_idx; i < chars; i++) {
        uint8_t a = *((uint8_t*)s1 + i);
        uint8_t b = *((uint8_t*)s2 + i);
        if(a < b) {
            order = -1;
            goto done;
        }
        else if(a > b) {
            order = 1;
            goto done;
        }
    }
done:
    return order;
}
