
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "editor.h"
#include "editor/buffer.h"
#include "editor/format.h"
#include "editor/input.h"

#include "terminal.h"

#include "global.h"

struct EditorConfig editor;

// This is a temporary file buffer to store the test operations
// TODO: Create a better structure to handle the open files
struct FileBuffer file_buffer = {NULL, NULL, 0, 0, BUF_INIT, BUF_INIT};

// Helper function to calculate rx from cx for a given line
int calculate_rx_from_cx() {
  struct CharBuffer *raw_line = &file_buffer.raw[editor.cy];
  int pos = 0;
  int rx = 0;

  while (pos < editor.cx && (size_t)rx < raw_line->len) {
    char c = raw_line->buf[rx];

    if (c == '\t') {
      pos += TABULATION_SIZE - (pos % TABULATION_SIZE);
      if (pos > editor.cx)
        break;
    } else {
      pos++;
    }

    rx++;
  }

  return rx;
}

// Helper function to calculate cx from rx for a given line
int calculate_cx_from_rx() {
  struct CharBuffer *raw_line = &file_buffer.raw[editor.cy];
  int cx = 0;

  for (int i = 0; i < editor.rx && (size_t)i < raw_line->len; i++) {
    char c = raw_line->buf[i];

    if (c == '\t') {
      cx += TABULATION_SIZE - (cx % TABULATION_SIZE);
    } else {
      cx++;
    }
  }

  return cx;
}

void editor_init() {
  editor.cx = 0;
  editor.cy = 0;
  editor.rx = 0;
  editor.cols_offset = 0;
  editor.rows_offset = 0;

  if (get_window_size(&editor.screen_rows, &editor.screen_cols) == -1)
    panic("get_window_size");

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

  if (CharBuffer_init(&editor.message) == -1)
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

  if (CharBuffer_init(&ab) == -1)
    panic("Creating the screen buffer");

  // hide the cursor during the draw
  CharBuffer_append_text(&ab, "\x1b[?25l", 6);
  // put the cursor on the top left
  CharBuffer_append_text(&ab, "\x1b[H", 3);

  editor_draw_rows(&ab);
  editor_draw_status_line(&ab);
  editor_show_message(&ab);

  // replace the cursor at its normal position
  char buf[32];
  int screen_cx = editor.cx - editor.cols_offset + editor.margins;
  if (screen_cx < editor.margins)
    screen_cx = editor.margins;

  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", editor.cy - editor.rows_offset + 1,
           screen_cx + 1);
  CharBuffer_append_text(&ab, buf, strlen(buf));
  // show the cursor
  CharBuffer_append_text(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.buf, ab.len);
  CharBuffer_free(&ab);
}

void editor_move_cursor(int key) {
  struct CharBuffer *raw_line = &file_buffer.raw[editor.cy];

  // This variable will allow for that one feature where you can keep the cursor at the end of the lines 
  // even when going from a short line to a long one
  int end_of_line = ((size_t)editor.rx == raw_line->len) ? 1 : 0; 
	  
  switch (key) {
  case ARROW_LEFT:
    if (editor.rx > 0) {
      editor.rx--;
    } else if (editor.cy > 0) {
      // Move to end of previous line
      editor.cy--;
      editor.rx = file_buffer.raw[editor.cy].len;
    }
    break;
  case ARROW_RIGHT:
    if (raw_line && (size_t)editor.rx < raw_line->len) {
      editor.rx++;
    } else if (raw_line && (size_t)editor.rx == raw_line->len &&
               (size_t)editor.cy < file_buffer.len - 1) {
      // Move to beginning of next line
      editor.cy++;
      editor.rx = 0;
    }
    break;
  case ARROW_UP:
    if (editor.cy > 0) {
      editor.cy--;
    }
    break;
  case ARROW_DOWN:
    if ((size_t)editor.cy < file_buffer.len - 1) {
      editor.cy++;
    }
    break;
  }

 // Ensure rx is within bounds
  raw_line = &file_buffer.raw[editor.cy];
 
  if (end_of_line && (key == ARROW_DOWN || key == ARROW_UP)) {
   // editor.rx = raw_line->len;
  }
                                                  
  if (raw_line && (size_t)editor.rx > raw_line->len) {
    editor.rx = raw_line->len;
  }

  editor.cx = calculate_cx_from_rx();
}

// Delete a single character from the raw file buffer
void editor_delete_character(int key) {
  if (key == BACKSPACE) {
    if (editor.rx == 0 && editor.cy > 0) {
      // Position cursor at the merge point
      editor.rx = file_buffer.raw[editor.cy - 1].len;
      // Merge current line with previous line
      CharBuffer_append_text(&file_buffer.raw[editor.cy - 1],
                             file_buffer.raw[editor.cy].buf,
                             file_buffer.raw[editor.cy].len);

      FileBuffer_remove_lines(&file_buffer, editor.cy, 1);
      editor.cy--;

      editor.cx = calculate_cx_from_rx();

      format_raw_text(&file_buffer.raw[editor.cy],
                      &file_buffer.format[editor.cy]);
      return;
    }

    if (editor.rx > 0) {
      editor_move_cursor(ARROW_LEFT);
    } else {
      return; // Nothing to delete
    }
  } else if (key == DEL_KEY) {
    if ((size_t)editor.rx >= file_buffer.raw[editor.cy].len) {
      // Delete at end of line - merge with next line
      if ((size_t)editor.cy < file_buffer.len - 1) {
        CharBuffer_append_text(&file_buffer.raw[editor.cy],
                               file_buffer.raw[editor.cy + 1].buf,
                               file_buffer.raw[editor.cy + 1].len);

        FileBuffer_remove_lines(&file_buffer, editor.cy + 1, 1);
        format_raw_text(&file_buffer.raw[editor.cy],
                        &file_buffer.format[editor.cy]);
      }
      return;
    }
  }

  CharBuffer_remove_chars(&file_buffer.raw[editor.cy], editor.rx, 1);
  format_raw_text(&file_buffer.raw[editor.cy], &file_buffer.format[editor.cy]);
}

void editor_process_keypress() {
  int c = editor_read_key();

  // Printable characters (including tab)
  if ((c >= 32 && c <= 126) || c == '\t') {
    // Insert the character at current rx position
    CharBuffer_insert_text(&file_buffer.raw[editor.cy], (char *)&c, editor.rx,
                           1);

    // Update rx and cx
    editor.rx++;
    if (c == '\t') {
      // Tab character inserted - calculate new cx
      editor.cx = calculate_cx_from_rx();
    } else {
      editor.cx++;
    }

    // Reformat the line
    format_raw_text(&file_buffer.raw[editor.cy],
                    &file_buffer.format[editor.cy]);
    return;
  }

  // Command characters
  switch (c) {
  // Ctrl+Q -> exit
  case CTRL_KEY('q'):
    clear_screen();
    reset_cursor_position();
    exit(0);
    break;

  // Ctrl+S -> save file
  case CTRL_KEY('s'):
    editor_save_file(&file_buffer);
    break;

  case PAGE_UP:
  case PAGE_DOWN: {
    int to_be_scrolled = editor.rows - 2;
    if (c == PAGE_UP) {
      editor.cy = editor.rows_offset;
      while (to_be_scrolled-- && editor.cy > 0) {
        editor.cy--;
      }
    } else {
      // PAGE_DOWN
      editor.cy = editor.rows_offset + editor.rows - 1;
      if ((size_t)editor.cy >= file_buffer.len)
        editor.cy = file_buffer.len - 1;
      while (to_be_scrolled-- &&
             (unsigned int)editor.cy < file_buffer.len - 1) {
        editor.cy++;
      }
    }

    // Keep rx within bounds of new line
    if ((size_t)editor.rx > file_buffer.raw[editor.cy].len) {
      editor.rx = file_buffer.raw[editor.cy].len;
    }
    editor.cx = calculate_cx_from_rx();
    break;
  }

  case HOME_KEY:
    editor.rx = 0;
    editor.cx = 0;
    break;

  case END_KEY:
    editor.rx = file_buffer.raw[editor.cy].len;
    editor.cx = calculate_cx_from_rx();
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
  case ENTER: {
    // Create a new line and split the current one in two
    FileBuffer_insert_text(&file_buffer, "\n", editor.cy, editor.rx, 1);
    editor_move_cursor(ARROW_RIGHT);

  } break;
  }
}

void editor_on_exit() {
  if (file_buffer.capacity > 0)
    FileBuffer_free(&file_buffer);
}
