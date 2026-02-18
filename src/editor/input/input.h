#ifndef EDITOR_INPUT_H
#define EDITOR_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif


/** Macro time **/
// maps the lowercase letter 'x' to the Ctrl+x command
#define CTRL_KEY(k) ((k) & 0x1f)

#define BACKSPACE 127 
#define ENTER 13

enum EditorKey {
  ARROW_RIGHT=1000,
  ARROW_LEFT,
  ARROW_UP,
  ARROW_DOWN,

  PAGE_UP,
  PAGE_DOWN,  
  HOME_KEY,
  END_KEY,

  DEL_KEY,


  NOP

};

/*
This function was originaly in terminal.c but was moved here for consistancy sake.
 */

/// Read the input from the keyboard and send the corresponding code to the editor
int editor_read_key();


#ifdef __cplusplus
}
#endif
#endif