#include <stdlib.h>
#include <string.h>

#include "../../global.h"
#include "../buffer.h"


typedef struct FileBuffer FileBuffer;
typedef struct CharBuffer CharBuffer;

int FileBuffer_init(struct FileBuffer *file_buffer) {

  void *raw = malloc(16 * sizeof(CharBuffer));
  void *format = malloc(16 * sizeof(CharBuffer));
  if (format == NULL || raw == NULL) {
    return -1;
  }

  if (CharBuffer_init(&file_buffer->path) == -1 ||
      CharBuffer_init(&file_buffer->file_name) == -1)
    return -1;

  file_buffer->raw = (CharBuffer *)raw;
  file_buffer->format = (CharBuffer *)format;
  file_buffer->capacity = 16;
  file_buffer->len = 0;

  return 0;
}

int FileBuffer_grow(struct FileBuffer *file_beffer, size_t needed) {
  if (file_beffer->len + needed + 1 <= file_beffer->capacity) {
    // We are chill
    return 0;
  }

  // grow by doubling the capacity
  size_t new_capacity = file_beffer->capacity;
  while (new_capacity < file_beffer->len + needed + 1)
    new_capacity *= 2;

  // the reallocations can fail
  CharBuffer *new_raw =
      realloc(file_beffer->raw, new_capacity * sizeof(CharBuffer));
  CharBuffer *new_format =
      realloc(file_beffer->format, new_capacity * sizeof(CharBuffer));
  if (new_format == NULL || new_raw == NULL)
    return -1;

  file_beffer->raw = new_raw;
  file_beffer->format = new_format;
  file_beffer->capacity = new_capacity;

  return 0;
}

int FileBuffer_add_line(FileBuffer *file_buffer) {
  if (FileBuffer_grow(file_buffer, 1) == -1) {
    return -1; // the reallocation failled
  }

  // We initialize the new line with a \0

  if (CharBuffer_init(&file_buffer->raw[file_buffer->len]) == -1 ||
      CharBuffer_init(&file_buffer->format[file_buffer->len]) == -1)
    return -1;

  file_buffer->len++;

  return 0;
}


void FileBuffer_remove_line(struct FileBuffer *file_buffer, size_t index) {
  // some bound checking to be safe
  if (index >= file_buffer->len)
    return;

  // Unallocate the buffers first
  CharBuffer_free(&file_buffer->raw[index]);
  CharBuffer_free(&file_buffer->format[index]);

  // number of bytes to be copied
  size_t count = file_buffer->len - (index + 1);
  memmove(&file_buffer->raw[index], &file_buffer->raw[index+1], count * sizeof(CharBuffer));
  memmove(&file_buffer->format[index], &file_buffer->format[index+1], count * sizeof(CharBuffer));

  file_buffer->len--;
}

void FileBuffer_append_text(struct FileBuffer *file_buffer, const char *s) {

  // First, we need to add a new line to the filebuffer
  if (file_buffer->len == 0) {
    if (FileBuffer_add_line(file_buffer) == -1) {
      panic("FileBuffer_append_text, adding a new line");
    }
  }

  // Time to parse the content, and add it to the file buffer
  char c = s[0];
  int i = 0;

  int last_line = file_buffer->len - 1;

  while ((c = s[i++]) != '\0') {
    // If we hit a newline character, we need to create a new line
    if (c == '\n') {

      if (FileBuffer_add_line(file_buffer) == -1)
        panic("FileBuffer_append_text, adding a new line");

      last_line++;
    } else {
      CharBuffer_append_text(&(file_buffer->raw[last_line]), &c, 1);
    }
  }
}

void FileBuffer_free(FileBuffer *file_buffer) {
  // Clean each line buffer
  for (unsigned int i = 0; i < file_buffer->len; i++) {
    CharBuffer_free(&file_buffer->raw[i]);
    CharBuffer_free(&file_buffer->format[i]);
  }

  // make sure to not leave any garbage values in the struct
  free(file_buffer->raw);
  free(file_buffer->format);
  file_buffer->raw = NULL;
  file_buffer->format = NULL;
  file_buffer->len = 0;
}