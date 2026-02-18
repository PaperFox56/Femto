#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "editor/editor.h"

#include "common/functions.h"
#include "common/macros.h"

#include "terminal/terminal.h"

static const char help_str[] =
    "Femto editor version " FEMTO_VERSION " - Help text\n"
    "Usage:\n"
    "    femto [file]\n";

void clean_and_exit() { editor_on_exit(); }

int main(int args, char **argv) {

  // Parse command line arguments
  if (args > 1) {
    if (argv[1][0] == '-') {
      // Display an help message
      printf(help_str);
      exit(0);
    } else {

      editor_open_file(argv[1]);
    }
  }

  // security mesure
  atexit(clean_and_exit);
  enable_raw_mode();

  // Main stuff
  editor_init();

  while (1) {
    editor_refresh_screen();
    editor_process_keypress();
  }

  return 0;
}

void panic(const char *s) {
  clear_screen();
  reset_cursor_position();

  perror(s);
  exit(1);
}

