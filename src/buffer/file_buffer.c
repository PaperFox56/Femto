#include <stdlib.h>

#include "../buffer.h"
#include "../global.h"

typedef struct FileBuffer FileBuffer;
typedef struct CharBuffer CharBuffer;


int FileBuffer_init(struct FileBuffer* file_buffer) {

  void *new = malloc(16 * sizeof(CharBuffer));
  if (new == NULL) {
    return -1;
  }

  file_buffer->lines = (CharBuffer *)new;
  file_buffer->capacity = 16;
  file_buffer->len = 0;

  return 0;
}

int FileBuffer_grow(struct FileBuffer* file_beffer, size_t needed) {
  if (file_beffer->len + needed + 1 <= file_beffer->capacity) {
    // We are chill
    return 0;
  }

  // grow by doubling the capacity
  size_t new_capacity = file_beffer->capacity;
  while (new_capacity < file_beffer->len + needed + 1)
    new_capacity *= 2;

  // the reallocation can fail
  CharBuffer *new_buf = realloc(file_beffer->lines, new_capacity * sizeof(CharBuffer));
  if (!new_buf)
    return -1;

  file_beffer->lines = new_buf;
  file_beffer->capacity = new_capacity;

  return 0;
}

int FileBuffer_add_line(FileBuffer *file_buffer) {
  if (FileBuffer_grow(file_buffer, 1) == -1) {
    return -1; // the reallocation failled
  }

  // We initialize the new line with a \0

  CharBuffer_init(&file_buffer->lines[file_buffer->len]);

  file_buffer->len++;

  return 0;
}

int FileBuffer_append_text(struct FileBuffer *file_buffer, const char *s) {

  // First, we need to add a new line to the filebuffer
  if (file_buffer->len == 0) {
    FileBuffer_add_line(file_buffer);
  }

  // Time to parse the content, and add it to the file buffer
  char c = s[0];
  int i = 0;

  int last_line = file_buffer->len - 1;

  while ((c = s[i++]) != '\0') {
    // If we hit a newline character, we need to create a new line
    if (c == '\n') {
      
      if (FileBuffer_add_line(file_buffer) == -1)
        panic("Adding a line to a file buffer");

      last_line++;
    } else {
      CharBuffer_append_text(&(file_buffer->lines[last_line]), &c, 1);
    }
  }

  return 0;
}

void FileBuffer_free(FileBuffer *file_buffer) {
  // Clean each line buffer
  for (unsigned int i = 0; i < file_buffer->len; i++) {
    CharBuffer_free(&file_buffer->lines[i]);
  }

  // make sure to not leave any garbage values in the struct
  free(file_buffer->lines);
  file_buffer->lines = NULL;
  file_buffer->len = 0;
}