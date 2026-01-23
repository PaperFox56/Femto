#ifndef EDITOR_H
#define EDITOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "terminal.h"

#define clear_screen() write(STDOUT_FILENO, "\x1b[2J", 4)
#define reset_cursor_position() write(STDOUT_FILENO, "\x1b[H", 3)

struct EditorConfig {
  // cursor position
  int cx;
  int cy;

  // screen dimensions
  int screen_rows;
  int screen_cols;
};

void initEditor();

/// Cleans the entire terminal screen and position the cursor at the top left
void editor_clean_screen();
// Redraws the screen
void editor_refresh_screen();
void editor_process_keypress();

// File management
// Open a file, read it's content, and fill a new file buffer
void editor_open_file(const char *path);

// Operations to do when the editor exit, whether it's caused by an error or not
void editor_on_exit();

#ifdef __cplusplus
}
#endif
#endif