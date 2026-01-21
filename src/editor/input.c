#include <errno.h>
#include <unistd.h>

#include "../global.h"
#include "input.h"

int editorReadKey() {
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      panic("read");
  }

  // Add support for escaped sequences
  if (c == '\x1b') {

    // we expect at most three characters
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    if (seq[0] == '[') {
      switch (seq[1]) {
      case 'A':
        return EDITOR_MOVE_CURSOR_UP;
      case 'B':
        return EDITOR_MOVE_CURSOR_DOWN;
      case 'C':
        return EDITOR_MOVE_CURSOR_RIGHT;
      case 'D':
        return EDITOR_MOVE_CURSOR_LEFT;
      }
    }
    return '\x1b';

  } else {
    return c;
  }
}
