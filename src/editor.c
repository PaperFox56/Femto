// Because of the `getline` function
#include <stddef.h>
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"
#include "editor.h"
#include "editor/input.h"

#include "global.h"

struct EditorConfig editor;

// This is a temporary file buffer to store the test operations
// TODO: Create a better structure to handle the open files
struct FileBuffer file_buffer = {NULL};

void init_editor() {
  if (getWindowSize(&editor.screen_rows, &editor.screen_cols) == -1)
    panic("getWindowSize");

  editor.rows = editor.screen_rows;
  editor.cols = editor.screen_cols - editor.margins;
}

/// This function is called by `editor_refresh_screen`. It ensures the drawing
/// of each row of the editor
void editorDrawRows(struct CharBuffer *ab) {
  int margin = editor.margins;
  int y;

  for (y = 0; y < editor.rows; y++) {
    unsigned int file_row = y + editor.rows_offset;

    // TODO: When we implement the filemanager, we should instead check whether
    // a file is open or not
    if (file_buffer.len > 0) {
      if (file_row < file_buffer.len) {
        // Line number

        char line_number_str[12];
        sprintf(line_number_str, "%d ", file_row + 1);

        int size = strlen(line_number_str);

        // color the text in light gray
        CharBuffer_append_text(ab, "\033[38;2;175;175;175m", 19);
        // add some space ' ' character to align the digits
        for (int i = 0; i < margin - size; i++)
          CharBuffer_append_text(ab, " ", 1);
        CharBuffer_append_text(ab, line_number_str, size);
        // reset the color
        CharBuffer_append_text(ab, "\x1b[0m", 4);

        // Print the line
        // TODO: Take in account the fact that the line might be too long to
        // display
        int len = file_buffer.lines[file_row].len - editor.cols_offset;
        if (len < 0)
          len = 0;
        if (len > editor.cols)
          len = editor.cols;

        CharBuffer_append_text(
            ab, file_buffer.lines[file_row].buf + editor.cols_offset, len);
      } else {
        CharBuffer_append_text(ab, "~", 1);
      }
    } else {

      // Print a welcome message

      if (y == editor.screen_rows / 3) {
        char welcome[80];

        int welcomelen = snprintf(welcome, sizeof(welcome),
                                  "Femto editor -- version %s", FEMTO_VERSION);

        if (welcomelen > editor.screen_cols)
          welcomelen = editor.screen_cols;

        // center the text
        int padding = (editor.screen_cols - welcomelen) / 2;
        if (padding) {
          CharBuffer_append_text(ab, "~", 1);
          padding--;
        }
        while (padding--)
          CharBuffer_append_text(ab, " ", 1);
        CharBuffer_append_text(ab, welcome, welcomelen);
      } else {
        CharBuffer_append_text(ab, "~", 1);
      }
    }

    // clean the rest of the lines
    CharBuffer_append_text(ab, "\x1b[K", 3);
    if (y < editor.rows - 1)
      CharBuffer_append_text(ab, "\r\n", 3);
  }
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

  editorDrawRows(&ab);

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
                                ? &file_buffer.lines[editor.cy]
                                : NULL;

  switch (key) {
  case ARROW_LEFT:
    if (editor.cx > 0)
      editor.cx--;
    else if (editor.cy > 0) {
      // This allows to go the the end of the next line from the start of the
      // current line
      editor.cy--;
      editor.cx = file_buffer.lines[editor.cy].len;
    }
    break;
  case ARROW_RIGHT:
    if ((unsigned int)editor.cx < line->len)
      editor.cx++;
    else if (line && (unsigned int)editor.cx == line->len && (unsigned int)editor.cy < file_buffer.len - 1) {
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
             ? &file_buffer.lines[editor.cy]
             : NULL;
  // Prevent the cursor from going past the end of the line
  if (line && (unsigned int)editor.cx >= line->len) {
    editor.cx = line->len;
  }
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
    int to_be_scrolled = editor.rows;
    while (to_be_scrolled--) {
      editor_move_cursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }

    editor.cx = 0;
  } break;

  case HOME_KEY:
    editor.cx = 0;
    break;
  case END_KEY:
    while (editor.cx < file_buffer.lines[editor.cy].len) { //  go to the end of the line
      editor_move_cursor(ARROW_RIGHT);
    }  break;

  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_RIGHT:
  case ARROW_LEFT:
    editor_move_cursor(c);
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

  while (getline(&line, &linecap, file) != -1) {
    FileBuffer_append_text(&file_buffer, line);

    if (file_buffer.len > MAX_LINE_COUNT) {
      free(line);
      fclose(file);

      panic("This file is too big to be opened in the editor");
    }
  }
  FileBuffer_add_line(&file_buffer);

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

  // We add an empty line at the end, always.

  free(line);
  fclose(file);
}

void editor_on_exit() {
  if (file_buffer.capacity > 0)
    FileBuffer_free(&file_buffer);
}