#ifndef EDITOR_H
#define EDITOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#include "editor/buffer.h"

#define clear_screen() write(STDOUT_FILENO, "\x1b[2J", 4)
#define reset_cursor_position() write(STDOUT_FILENO, "\x1b[H", 3)

struct EditorConfig {
  // cursor position in the formated text
  int cx;
  int cy;

  // cursor position in the raw text
  int rx;

  // scroll offsets
  int rows_offset;
  int cols_offset;

  int margins;

  // screen dimensions
  int screen_rows;
  int screen_cols;

  // editor dimensions (screen dimension minus some UI elements)
  int rows;
  int cols;

  // custom message
  struct CharBuffer message;
  time_t message_timeout;
};

void editor_init();

void editor_set_message(const char *s, ...);

/* Drawing functions */
// Draw the text form the file buffer on the screen allong with optional UI (lines numbers for example)
void editor_draw_rows(struct CharBuffer *ab);
//
void editor_draw_status_line(struct CharBuffer *ab);
void editor_show_message(struct CharBuffer *ab);

/// Cleans the entire terminal screen and position the cursor at the top left
void editor_clean_screen();
// Redraws the screen
void editor_refresh_screen();
void editor_process_keypress();

/* File management */
// Open a file, read it's content, and fill a new file buffer
void editor_open_file(const char *path);
// Save a file to the disk by overwriting it with the content of the file buffer
void editor_save_file(const struct FileBuffer *file_buffer);

// Operations to do when the editor exit, whether it's caused by an error or not
void editor_on_exit();

#ifdef __cplusplus
}
#endif
#endif