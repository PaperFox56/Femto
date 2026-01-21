#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "editor.h"
#include "global.h"

// Blow up the all thing and let the OS clean after us
void panic(const char *s) {
  clear_screen();
  reset_cursor_position();

  perror(s);
  exit(1);
}

int main() {

  enable_raw_mode();

  while (true) {
    initEditor();
    editor_refresh_screen();
    editor_process_keypress();
  }

  return 0;
}