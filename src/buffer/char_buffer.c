#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../buffer.h"

typedef struct FileBuffer FileBuffer;
typedef struct CharBuffer CharBuffer;

int CharBuffer_init(struct CharBuffer *char_buffer) {
  void *new = malloc(16 * sizeof(char));
  if (new == NULL) {
    return -1;
  }

  char_buffer->buf = (char *)new;
  char_buffer->capacity = 16;
  char_buffer->len = 0;

  char_buffer->buf[0] = '\0';

  return 0;
}

int CharBuffer_grow(struct CharBuffer *char_buffer, size_t needed) {
  if (char_buffer->len + needed + 1 <= char_buffer->capacity) {
    // We are chill
    return 0;
  }

  // grow by doubling the capacity
  size_t new_capacity = char_buffer->capacity;
  while (new_capacity < char_buffer->len + needed + 1)
    new_capacity *= 2;

  // the reallocation can fail
  char *new_buf = realloc(char_buffer->buf, new_capacity);
  if (!new_buf)
    return -1;

  char_buffer->buf = new_buf;
  char_buffer->capacity = new_capacity;

  return 0;
}

int CharBuffer_append_text(CharBuffer *cb, const char *s, size_t len) {
  if (CharBuffer_grow(cb, len) == -1)
    return -1;

  memcpy(&cb->buf[cb->len], s, len);

  cb->len += len;
  cb->buf[cb->len] = '\0';

  return cb->len;
}

void CharBuffer_free(CharBuffer *char_buffer) {
  free(char_buffer->buf); // free the internal buffer

  char_buffer->buf = NULL;
  char_buffer->len = 0;
}
