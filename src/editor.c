#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "editor.h"
#include "editor/buffer.h"
#include "editor/input.h"

#include "global.h"


struct EditorConfig editor;

void initEditor() {
  if (getWindowSize(&editor.screen_rows, &editor.screen_cols) == -1)
    panic("getWindowSize");
}

/// This function is called by `editor_refresh_screen`. It ensures the drawing
/// of each row of the editor
void editorDrawRows(struct AppBuffer *ab) {
  int y;
  for (y = 0; y < editor.screen_rows; y++) {

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
        abAppend(ab, "~", 1);
        padding--;
      }
      while (padding--)
        abAppend(ab, " ", 1);
      abAppend(ab, welcome, welcomelen);
    } else {
      abAppend(ab, "~", 1);
    }

    // clean
    abAppend(ab, "\x1b[K", 3);
    if (y < editor.screen_rows - 1)
      abAppend(ab, "\r\n", 3);
  }
}

void editor_refresh_screen() {
  struct AppBuffer ab = ABUF_INIT;

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
    case EDITOR_MOVE_CURSOR_LEFT:
      if (editor.cx > 0) editor.cx--;
      break;
    case EDITOR_MOVE_CURSOR_RIGHT:
      if (editor.cx < editor.screen_cols-1) editor.cx++;
      break;
    case EDITOR_MOVE_CURSOR_UP:
      if (editor.cy > 0) editor.cy--;
      break;
    case EDITOR_MOVE_CURSOR_DOWN:
      if (editor.cy < editor.screen_rows-1) editor.cy++;
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

  case EDITOR_MOVE_CURSOR_UP:
  case EDITOR_MOVE_CURSOR_DOWN:
  case EDITOR_MOVE_CURSOR_RIGHT:
  case EDITOR_MOVE_CURSOR_LEFT:
    editorMoveCursor(c);
    break;
  }
}
