#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "editor.h"
#include "global.h"

static const char *log_file_path = "femto.log";


// Blow up the all thing and let the OS clean after us
void panic(const char *s) {
  clear_screen();
  reset_cursor_position();

  perror(s);
  exit(1);
}

void print_log(const char* restrict format, ...) {
  // Open the log file
  FILE *log_file = NULL;
  if ((log_file = fopen(log_file_path, "a")) == NULL) {
    perror("opening the log file");
  }

  va_list params;
  va_start(params, format);

  if (vfprintf(log_file, format, params) == -1) {
    perror("printing in the log file");
  }

  va_end(params);

  fclose(log_file);
}

void clean_and_exit() {

  editor_on_exit();
}

int main(int args, char** argv) {
  // Clean the logs
  FILE *log_file = NULL;
  if ((log_file = fopen(log_file_path, "w")) == NULL) {
    perror("opening the log file");
  }
  fclose(log_file);


  // Main stuff
  atexit(clean_and_exit);

  enable_raw_mode();

  editor_init();

  // Let's open a test file 
  if (args > 1)
    editor_open_file(argv[1]);

  while (1) {
    editor_refresh_screen();
    editor_process_keypress();
  }

  return 0;
}