
#include "buffer/char_buffer.h"
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include "../editor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void panic(const char *s);

// These variables are declared in `editor.c`
extern struct FileBuffer file_buffer;
extern struct EditorConfig editor;

void editor_open_file(const char *path) {
  // For now we will put an arbitrary text in there

  // Initialize the file buffer
  FileBuffer_init(&file_buffer);

  FILE *file = fopen(path, "r");
  if (!file)
    panic("fopen");

  char *line = NULL;
  size_t linecap = 0;
  int line_size = 0;

  CharBuffer_append_text(&file_buffer.path, path, strlen(path));
  CharBuffer_append_text(&file_buffer.file_name, path, strlen(path));

  while ((line_size = getline(&line, &linecap, file)) != -1) {
    FileBuffer_append_text(&file_buffer, line, line_size);

    if (file_buffer.len > MAX_LINE_COUNT) {
      free(line);
      fclose(file);

      panic("This file is too big to be opened in the editor");
    }
  }

  if (file_buffer.len == 0)
    FileBuffer_add_line(&file_buffer);

  free(line);
  fclose(file);
}

void editor_save_file(const struct FileBuffer *file_buffer) {
  FILE *file = fopen(file_buffer->path.buf, "w");
  if (!file)
    panic("editor_save_file");

  for (size_t i = 0; i < file_buffer->len; i++) {
    struct CharBuffer *line = &file_buffer->raw[i];
    fprintf(file, "%s\n", line->buf);

  }

  editor_set_message("File `%s` was saved succesfully", file_buffer->path.buf);

  fclose(file);
}
