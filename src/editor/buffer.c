#include <string.h>
#include <stdlib.h>

#include "buffer.h"

void abAppend(struct AppBuffer *ab, const char *s, int len) {
  // TODO: avoid the systematic reallocation by adding a `capacity` field
  char *new = realloc(ab->buf, ab->len + len);

  if (new == NULL) return; // something went wrong

  memcpy(&new[ab->len], s, len);
  ab->buf = new;
  ab->len += len;
}

void abFree(struct AppBuffer *ab) {
  free(ab->buf);
}