#include <errno.h>
#include <unistd.h>

#include "../../common/functions.h"
#include "input.h"

int editor_read_key() {
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      panic("read");
  }

  // if (nread == -1)
  //   return NOP;

  // Add support for escaped sequences
  if (c == '\x1b') {

    // we expect at most three characters
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    if (seq[0] == '[') {

      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1)
          return '\x1b';

        // Special keys
        if (seq[2] == '~') {
          switch (seq[1]) {
          case '1':
            return HOME_KEY;
          case '3':
            return DEL_KEY;
          case '4':
            return END_KEY;

          case '5':
            return PAGE_UP;
          case '6':
            return PAGE_DOWN;

          case '7':
            return HOME_KEY;
          case '8':
            return END_KEY;
          }
        }
      } else {
        // Arrow keys
        switch (seq[1]) {
        case 'A':
          return ARROW_UP;
        case 'B':
          return ARROW_DOWN;
        case 'C':
          return ARROW_RIGHT;
        case 'D':
          return ARROW_LEFT;
        case 'H':
          return HOME_KEY;
        case 'F':
          return END_KEY;
        }
      }
    } else if (seq[0] == 'O') { // Can't they just agree on something for once ?
      switch (seq[1]) {
      case 'H':
        return HOME_KEY;
      case 'F':
        return END_KEY;
      }
    }
    return '\x1b';

  } else {

    switch (c) {
    case CTRL_KEY('p'):
      return ARROW_UP;
    case CTRL_KEY('n'):
      return ARROW_DOWN;
    case CTRL_KEY('f'):
      return ARROW_RIGHT;
    case CTRL_KEY('b'):
      return ARROW_LEFT;
  }
    return c;
  }
}
