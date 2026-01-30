#include "format.h"
#include "buffer/char_buffer.h"

#include <stdlib.h>

typedef struct CharBuffer CharBuffer;

CharBuffer* format_raw_text(const CharBuffer *raw, CharBuffer *dest) {
    // We should first check that the raw buffer is valid
    if (raw == NULL) {
        return NULL;
    } else {
        // Check the internal buffer
        if (raw->buf == NULL) {
            return NULL;
        }
    }

    if (dest == NULL) {
        // try to allocate a new buffer
        if ((dest = (CharBuffer*)malloc(sizeof(CharBuffer))) == NULL)
            return NULL;
    
        if (CharBuffer_init(dest) == -1) {
            free(dest);
            return NULL;
        }
    }

    // Here we are sure that `raw` and `dest` are valid CharBuffer pointers

    // For now we just copy the raw buffer

    dest->len = 0; // empty the buffer
    dest->buf[0] = '\0';

    // position on the line
    int pos = 0;
    for (unsigned int i = 0; i < raw->len; i++) {
        char c = raw->buf[i];

        switch (c) {
            case '\t': {
                int gap = TABULATION_SIZE - (pos % TABULATION_SIZE);
                CharBuffer_append_text(dest, "    ", gap);
                pos += gap;
            } break;
            default:
                CharBuffer_append_text(dest, &c, 1);
                pos++;
        }

    }

    return dest;
}
