/**
Implements files handeling the loging system
*/

#include <stdarg.h>
#include <stdio.h>

#include "functions.h"

#define USE_LOGS

#define LOG_IN_FILE // define whether to use an actual file or the standard
                    // output buffer

static const char *log_file_path = "femto.log";

void init_log() {
#ifdef USE_LOGS
#ifdef LOG_IN_FILE

  // Clean the logs
  FILE *log_file = NULL;
  if ((log_file = fopen(log_file_path, "w")) == NULL) {
    perror("opening the log file");
  }
  fclose(log_file);

#endif
#endif
}

void print_log(const char *restrict format, ...) {

#ifdef USE_LOGS
#ifdef LOG_IN_FILE
  // Open the log file
  FILE *log_file = NULL;
  if ((log_file = fopen(log_file_path, "a")) == NULL) {
    perror("opening the log file");
  }
#else
  FILE *log_file = stdout;
#endif

  va_list params;
  va_start(params, format);

  if (vfprintf(log_file, format, params) == -1) {
    perror("printing in the log file");
  }

  va_end(params);

#ifdef LOG_IN_FILE
  fclose(log_file);
#endif
#endif
}
