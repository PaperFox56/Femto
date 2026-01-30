#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../global.h"
#include "buffer.h"
#include "format.h"


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


void FileBuffer_remove_lines(struct FileBuffer *file_buffer, size_t index, size_t len) {
  // some bound checking to be safe
  if (index + len > file_buffer->len)
    return;

  // Unallocate the buffers first
  for (size_t i = index; i < index + len; i++) {
    CharBuffer_free(&file_buffer->raw[i]);
    CharBuffer_free(&file_buffer->format[i]);
  }

  // number of bytes to be copied
  size_t count = file_buffer->len - (index + len);
  memmove(&file_buffer->raw[index], &file_buffer->raw[index+len], count * sizeof(CharBuffer));
  memmove(&file_buffer->format[index], &file_buffer->format[index+len], count * sizeof(CharBuffer));

  file_buffer->len -= len;
}

int FileBuffer_insert_lines(struct FileBuffer *file_buffer, size_t index, size_t len) {

  if (index > file_buffer->len)
    return -1;

  if (FileBuffer_grow(file_buffer, 1) == -1) {
    return -1; // the reallocation failled
  }

  // first make some room for the new text
  size_t count = file_buffer->len - index;
  memmove(&file_buffer->raw[index + len], &file_buffer->raw[index], count);
  memmove(&file_buffer->format[index + len], &file_buffer->format[index], count);

  // We initialize the new lines with a \0
  for (size_t i = index; i < index+len; i++) {
    if (CharBuffer_init(&file_buffer->raw[i]) == -1 ||
        CharBuffer_init(&file_buffer->format[i]) == -1)
      // At that point we have a really big issue, we have to terminate the program
      panic("FileBuffer_insert_lines, initialising the new buffers");
  }

  file_buffer->len += len;

  return 0;
}

void FileBuffer_insert_text(FileBuffer *file_buffer, const char *s, size_t line, size_t index, size_t maxlen) {
  if (line >= file_buffer->len) 
    return;

  if (index >= file_buffer->raw[line].len)
    return;

  // Time to parse the content, and add it to the file buffer
  char c = s[0];
  size_t i = 0;

  // the buffer should at leat contain a line at this point 
  size_t last_line = file_buffer->len - 1;

  size_t start = 0;

  while ((c = s[i]) != '\0' && i < maxlen) {
    // If we hit a newline character, we need to create a new line
    if (c == '\n' || c == '\r') {
      // Copy all the buffered tex
      CharBuffer_append_text(&(file_buffer->raw[last_line]), &s[start], i - start);

      if (FileBuffer_add_line(file_buffer) == -1)
        panic("FileBuffer_append_text, adding a new line");

      last_line++;

      start = i+1;
    }

    i++;
  }
}

void FileBuffer_append_text(struct FileBuffer *file_buffer, const char *s, size_t maxlen) {

  // First, we need to add a new line to the filebuffer
  if (file_buffer->len == 0) {
    if (FileBuffer_add_line(file_buffer) == -1) {
      panic("FileBuffer_append_text, adding a new line");
    }
  }

  // Time to parse the content, and add it to the file buffer
  char c = s[0];
  size_t i = 0;

  // the buffer should at leat contain a line at this point 
  size_t first_line = file_buffer->len - 1; // We keep track of this one to reformat every modified line later 
  size_t last_line = first_line;

  size_t start = 0;

  while ((c = s[i]) != '\0' && i < maxlen) {
    // If we hit a newline character, we need to create a new line
    if (c == '\n' || c == '\r') {
      // Copy all the buffered tex
      CharBuffer_append_text(&(file_buffer->raw[last_line]), &s[start], i - start);

      if (FileBuffer_add_line(file_buffer) == -1)
        panic("FileBuffer_append_text, adding a new line");

      last_line++;

      start = i+1;
    }

    i++;
  }

  // Now let's create the format buffers for display
  for (unsigned int i = first_line; i < file_buffer->len; i++) {
    format_raw_text(&file_buffer->raw[i], &file_buffer->format[i]);
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