// Because of the `getline` function
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "editor.h"
#include "buffer.h"
#include "editor/input.h"

#include "global.h"

struct EditorConfig editor;

// This is a temporary file buffer to store the test operations
// TODO: Create a better structure to handle the open files
struct FileBuffer file_buffer = {NULL, 0, 0};

void initEditor() {
  if (getWindowSize(&editor.screen_rows, &editor.screen_cols) == -1)
    panic("getWindowSize");
}

/// This function is called by `editor_refresh_screen`. It ensures the drawing
/// of each row of the editor
void editorDrawRows(struct AppBuffer *ab) {
  int y;
  for (y = 0; y < editor.screen_rows; y++) {

    // TODO: When we implement the filemanager, we should instead check whether a file is open or not
    if (abs(y) < file_buffer.len) {
      // Line number

      char line_number[12];
      sprintf(line_number, "%d ", y);

      // This space will be taken by the line number
      int margin = strlen(line_number);

      // color the text in light gray
      abAppend(ab, "\033[38;2;175;175;175m", 19);
      abAppend(ab, line_number, margin);
      // reset the color
      abAppend(ab, "\x1b[0m", 4);

      // Print the line
      // TODO: Take in account the fact that the line might be too long to display 
      int len = file_buffer.lines[y].len;
      if (len > editor.screen_cols -margin) len = editor.screen_cols-margin;
      abAppend(ab, file_buffer.lines[y].buf, len);
    } else {
      abAppend(ab, "~", 1);
    }

    // Print a welcome message
    /*
    if (y == editor.screen_rows / 3) {
      char welcome[80];

      int welcomelen = snprintf(welcome, sizeof(welcome),
                                "Femto editor -- version %s", FEMTO_VERSION);

      if (welcomelen > editor.screen_cols)
        welcomelen = editor.screen_cols;

      // center the text
      int padding = (editor.screen_cols - welcomelen) / 2;
      if (padding) {
        abAppend(ab, "~", 1);
        padding--;
      }
      while (padding--)
        abAppend(ab, " ", 1);
      abAppend(ab, welcome, welcomelen);
    } else {
      abAppend(ab, "~", 1);
    }*/


    // clean
    abAppend(ab, "\x1b[K", 3);
    if (y < editor.screen_rows - 1)
      abAppend(ab, "\r\n", 3);
  }
}

void editor_refresh_screen() {
  struct AppBuffer ab = BUF_INIT;

  // hide the cursor during the draw
  abAppend(&ab, "\x1b[?25l", 6);
  // put the cursor on the top left
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab);

  // replace the cursor at its normal position
  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", editor.cy + 1, editor.cx + 1);
  abAppend(&ab, buf, strlen(buf));
  // show the cursor
  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.buf, ab.len);
  abFree(&ab);
}

void editorMoveCursor(int key) {
  switch (key) {
  case ARROW_LEFT:
    if (editor.cx > 0)
      editor.cx--;
    break;
  case ARROW_RIGHT:
    if (editor.cx < editor.screen_cols - 1)
      editor.cx++;
    break;
  case ARROW_UP:
    if (editor.cy > 0)
      editor.cy--;
    break;
  case ARROW_DOWN:
    if (editor.cy < editor.screen_rows - 1)
      editor.cy++;
    break;
  }
}

void editor_process_keypress() {
  int c = editorReadKey();
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
    int to_be_scrolled = editor.screen_rows;
    while (to_be_scrolled--) {
      editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }

    editor.cx = 0;
  } break;

  case HOME_KEY:
    editor.cx = 0;
    break;
  case END_KEY:
    editor.cx = editor.screen_cols - 1;
    break;

  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_RIGHT:
  case ARROW_LEFT:
    editorMoveCursor(c);
    break;
  }
}


void editor_open_file(const char *path) {
  // For now we will put an arbitrary text in there

  // Initialize the file buffer
  file_buffer.lines = (struct LineBuffer*)malloc(sizeof(struct LineBuffer));
  // Despite the a line buffer being allowed, we consider the buffer to be empty
  // The `FileBuffer_append_text` function will be in charge of creating the new line
  file_buffer.len = 0;


  FILE *file = fopen(path, "r");
  if (!file) panic("fopen");
  
  char *line = NULL;
  size_t linecap = 0;
  
  while (getline(&line, &linecap, file) != -1) {
    FileBuffer_append_text(&file_buffer, line);
  }

  free(line);
  fclose(file);

}

void editor_on_exit() {
  FileBuffer_free(&file_buffer);
}