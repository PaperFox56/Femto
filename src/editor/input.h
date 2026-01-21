#ifndef EDITOR_INPUT_H
#define EDITOR_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

enum EditorKey {
  EDITOR_MOVE_CURSOR_RIGHT=1000,
  EDITOR_MOVE_CURSOR_LEFT,
  EDITOR_MOVE_CURSOR_UP,
  EDITOR_MOVE_CURSOR_DOWN,

  EDITOR_PAGE_UP,
  EDITOR_PAGE_DOWN

};

/*
This function was originaly in terminal.c but was moved here for consistancy sake.
 */

/// Read the input from the keyboard and send the corresponding code to the editor
int editorReadKey();


#ifdef __cplusplus
}
#endif
#endif