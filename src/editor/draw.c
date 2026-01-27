#include "../editor.h"
#include "../global.h"
#include "buffer.h"

#include <stdio.h>
#include <string.h>

// These variables are declared in `editor.c`
extern struct FileBuffer file_buffer;
extern struct EditorConfig editor;

void editor_draw_rows(struct CharBuffer *ab) {
  int margin = editor.margins;
  int y;

  for (y = 0; y < editor.rows; y++) {
    unsigned int file_row = y + editor.rows_offset;
    // color the text in light gray
    CharBuffer_append_text(ab, "\033[38;2;180;180;180m", 19);

    // TODO: When we implement the filemanager, we should instead check whether
    // a file is open or not
    if (file_buffer.len > 0) {
      if (file_row < file_buffer.len) {
        // Line number

        char line_number_str[12];
        sprintf(line_number_str, "%d ", file_row + 1);

        int size = strlen(line_number_str);
        // add some space ' ' character to align the digits
        for (int i = 0; i < margin - size; i++)
          CharBuffer_append_text(ab, " ", 1);
        CharBuffer_append_text(ab, line_number_str, size);
        // reset the color
        CharBuffer_append_text(ab, "\x1b[m", 4);

        // Print the line
        // TODO: Take in account the fact that the line might be too long to
        // display
        int len = file_buffer.format[file_row].len - editor.cols_offset;
        if (len < 0)
          len = 0;
        if (len > editor.cols)
          len = editor.cols;

        CharBuffer_append_text(
            ab, file_buffer.format[file_row].buf + editor.cols_offset, len);
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
    CharBuffer_append_text(ab, "\r\n", 3);
    // reset color
    CharBuffer_append_text(ab, "\x1b[m", 4);
  }
}


void editor_draw_status_line(struct CharBuffer *ab) {

    // First, we invert the colors to have a white background
    CharBuffer_append_text(ab, "\x1b[7m", 4);

    // We will print the file_name, the row and column numberm s well as the
    // editor version

    unsigned int space_left = editor.screen_cols;

    struct CharBuffer temp = BUF_INIT;
    if (CharBuffer_init(&temp) == -1)
        return;

    CharBuffer_append_text(&temp, file_buffer.file_name.buf, file_buffer.file_name.len);

    char buf[32];

    snprintf(buf, 32, " (%d,%d)", editor.cy+1, editor.cx+1);
    CharBuffer_append_text(&temp, buf, strlen(buf));
    
    snprintf(buf, 32, "femto "FEMTO_VERSION);

    space_left -= temp.len + strlen(buf);
    for (unsigned int i = 0; i < space_left; i++) {
        CharBuffer_append_text(&temp, " ", 1);
    }
    
    CharBuffer_append_text(&temp, buf, strlen(buf));

    int min = temp.len > (unsigned int)editor.screen_cols ? temp.len : (unsigned int)editor.screen_cols;
    CharBuffer_append_text(ab, temp.buf, min);

    CharBuffer_append_text(ab, "\x1b[m", 4);
    CharBuffer_append_text(ab, "\r\n", 2);
}