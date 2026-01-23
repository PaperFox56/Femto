#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "global.h"

typedef struct FileBuffer FileBuffer;
typedef struct LineBuffer LineBuffer;

void abAppend(struct AppBuffer *ab, const char *s, int len) {
  // TODO: avoid the systematic reallocation by adding a `capacity` field
  char *new = realloc(ab->buf, ab->len + len);

  if (new == NULL)
    return; // something went wrong

  memcpy(&new[ab->len], s, len);
  ab->buf = new;
  ab->len += len;
}

void abFree(struct AppBuffer *ab) { free(ab->buf); }

void LineBuffer_append_text(LineBuffer *ab, const char *s, int len) {
  // TODO: avoid the systematic reallocation by adding a `capacity` field
  char *new = realloc(ab->buf, ab->len + len + 1);

  if (new == NULL)
    return; // something went wrong

  memcpy(&new[ab->len], s, len);
  ab->buf = new;

  ab->len += len;
  ab->buf[ab->len] = '\0';
}

void LineBuffer_free(LineBuffer *line_buffer) {
  free(line_buffer->buf); // free the internal buffer

  line_buffer->buf = NULL;
  line_buffer->len = 0;
}

// Insert a new line in a file buffer
int FileBuffer_add_line(FileBuffer *file_buffer) {

  if (file_buffer == NULL) {
    panic("Tried to add a line to an uninitalized file buffer");
  }

  LineBuffer *lines = NULL; // Store the new address of the reallocated array

  int new_capacity = (file_buffer->len + 1) * sizeof(LineBuffer);
  lines = realloc(file_buffer->lines, new_capacity);
  if (lines == NULL) {
    return -1; // the reallocation failled
  }

  // We initialize the new line with a \0
  unsigned int new_line_index = file_buffer->len;
  LineBuffer new_line = {(char *)malloc(sizeof(char)), 0};
  new_line.buf[0] = '\0';
  lines[new_line_index] = new_line;

  file_buffer->lines = lines;
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
      LineBuffer_append_text(&(file_buffer->lines[last_line]), &c, 1);
    }
  }

  return 0;
}

void FileBuffer_free(FileBuffer *file_buffer) {
  // Clean each line buffer
  for (unsigned int i = 0; i < file_buffer->len; i++) {
    LineBuffer_free(&file_buffer->lines[i]);
  }

  // make sure to not leave any garbage values in the struct
  free(file_buffer->lines);
  file_buffer->lines = NULL;
  file_buffer->len = 0;
}