// Because of the `getline` function
#include "editor/buffer.h"
#include "editor/format.h"
#include <stddef.h>
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "editor.h"
#include "editor/input.h"

#include "global.h"

struct EditorConfig editor;

// This is a temporary file buffer to store the test operations
// TODO: Create a better structure to handle the open files
struct FileBuffer file_buffer = {NULL, NULL, 0, 0, BUF_INIT, BUF_INIT};

void editor_init() {

  editor.cx = 0;
  editor.cy = 0;

  editor.rx = 0;

  editor.cols_offset = 0;
  editor.rows_offset = 0;

  if (getWindowSize(&editor.screen_rows, &editor.screen_cols) == -1)
    panic("getWindowSize");

  // The size of the margin is the number of digits in the biggest line number,
  // i.e. the line count. The best algorithm is the following (as long as the
  // maximum number of lines accepted is small enough)
  int margins = 1;
  int power_of_ten = 1;
  while (file_buffer.len >= (unsigned int)power_of_ten) {
    margins++;
    power_of_ten *= 10;
  }

  editor.margins = margins;

  editor.rows = editor.screen_rows - 2;
  editor.cols = editor.screen_cols - editor.margins;

  if (CharBuffer_init(&editor.message))
    panic("Initializing the message buffer");

  editor_set_message("Hello, welcome to femto editor !");
}

// Adjust the scrolling variables of the editor
void editor_scroll() {
  if (editor.cy < editor.rows_offset) {
    editor.rows_offset = editor.cy;
  }
  if (editor.cy >= editor.rows_offset + editor.rows) {
    editor.rows_offset = editor.cy - editor.rows + 1;
  }

  if (editor.cx < editor.cols_offset) {
    editor.cols_offset = editor.cx;
  }
  if (editor.cx >= editor.cols_offset + editor.cols) {
    editor.cols_offset = editor.cx - editor.cols + 1;
  }
}

void editor_refresh_screen() {

  editor_scroll();

  struct CharBuffer ab = BUF_INIT;

  CharBuffer_init(&ab);

  // hide the cursor during the draw
  CharBuffer_append_text(&ab, "\x1b[?25l", 6);
  // put the cursor on the top left
  CharBuffer_append_text(&ab, "\x1b[H", 3);

  editor_draw_rows(&ab);
  editor_draw_status_line(&ab);
  editor_show_message(&ab);

  // replace the cursor at its normal position
  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", editor.cy - editor.rows_offset + 1,
           editor.cx + editor.margins - editor.cols_offset + 1);
  CharBuffer_append_text(&ab, buf, strlen(buf));
  // show the cursor
  CharBuffer_append_text(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.buf, ab.len);
  CharBuffer_free(&ab);
}

void editor_move_cursor(int key) {

  struct CharBuffer *line = ((unsigned int)editor.cy < file_buffer.len)
                                ? &file_buffer.format[editor.cy]
                                : NULL;

  switch (key) {
  case ARROW_LEFT:
    if (editor.cx > 0)
      editor.cx--;
    else if (editor.cy > 0) {
      // This allows to go the the end of the next line from the start of the
      // current line
      editor.cy--;
      editor.cx = file_buffer.format[editor.cy].len;
    }
    break;
  case ARROW_RIGHT:
    if ((unsigned int)editor.cx < line->len)
      editor.cx++;
    else if (line && (unsigned int)editor.cx == line->len &&
             (unsigned int)editor.cy < file_buffer.len - 1) {
      // This allows to go to the begining of the next line
      editor.cy++;
      editor.cx = 0;
    }
    break;
  case ARROW_UP:
    if (editor.cy > 0)
      editor.cy--;
    break;
  case ARROW_DOWN:
    if ((unsigned int)editor.cy < file_buffer.len - 1)
      editor.cy++;
    break;

  }

  line = ((unsigned int)editor.cy < file_buffer.len)
             ? &file_buffer.format[editor.cy]
             : NULL;
  // Prevent the cursor from going past the end of the line
  if (line && (unsigned int)editor.cx >= line->len) {
    editor.cx = line->len;
  }

  // Since what is on the screen is different from the actual text, we need to
  // calculate `rx` based on `cx`
  line = &file_buffer.raw[editor.cy];
  int pos = 0;
  editor.rx = 0;
  while (pos < editor.cx) {
    char c = line->buf[editor.rx];

    if (c == '\t') {
      pos += TABULATION_SIZE - (pos % TABULATION_SIZE);
    } else {
      pos++;
    }

    editor.rx++;
  }
}

// Delete a single character form the raw file buffer
void editor_delete_character(int key) {
  if (key == BACKSPACE) {
    if (editor.cx == 0)
      return;
    else
     editor_move_cursor(ARROW_LEFT);
  }

  editor.cx--;
  CharBuffer_remove_chars(&file_buffer.raw[editor.cy], editor.rx, 1);
}

void editor_process_keypress() {
  int c = editor_read_key();
  switch (c) {

  // Ctrl+Q -> exit
  case CTRL_KEY('q'):
    clear_screen();
    reset_cursor_position();
    exit(0);
    break;

  case PAGE_UP:
  case PAGE_DOWN: {
    // Scroll to the top or the bottom of the page
    int to_be_scrolled = editor.rows - 2;
    editor.cx = 0;
    if (c == PAGE_UP) {
      editor.cy = editor.rows_offset + 1;
    } else {
      // place the cursor at the botom of the screen
      unsigned int min = editor.rows_offset + editor.rows - 1;
      min = min > file_buffer.len ? file_buffer.len - 1 : min - 1;
      editor.cy = min;
    }
    while (to_be_scrolled--) {
      editor_move_cursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }

    editor.cx = 0;
  } break;

  case HOME_KEY:
    editor.cx = 0;
    break;
  case END_KEY:
    while ((unsigned int)editor.cx <
           file_buffer.format[editor.cy].len) { //  go to the end of the line
      editor_move_cursor(ARROW_RIGHT);
    }
    break;

  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_RIGHT:
  case ARROW_LEFT:
    editor_move_cursor(c);
    break;

  case BACKSPACE:
  case DEL_KEY:
    editor_delete_character(c);
    break;  
  }

}

void editor_open_file(const char *path) {
  // For now we will put an arbitrary text in there

  // Initialize the file buffer
  FileBuffer_init(&file_buffer);

  FILE *file = fopen(path, "r");
  if (!file)
    panic("fopen");

  char *line = NULL;
  size_t linecap = 0;

  CharBuffer_append_text(&file_buffer.path, path, strlen(path));
  CharBuffer_append_text(&file_buffer.file_name, path, strlen(path));

  while (getline(&line, &linecap, file) != -1) {
    FileBuffer_append_text(&file_buffer, line);

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

  // Now let's create the format buffers for display
  for (unsigned int i = 0; i < file_buffer.len; i++) {
    format_raw_text(&file_buffer.raw[i], &file_buffer.format[i]);
  }
}

void editor_on_exit() {
  if (file_buffer.capacity > 0)
    FileBuffer_free(&file_buffer);
}
