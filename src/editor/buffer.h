#ifndef EDITOR_BUFFER_H
#define EDITOR_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

/// A buffer to which you can append text (yeah that's never been done before)
struct AppBuffer {
    char *buf;
    int len;
};

#define ABUF_INIT {NULL, 0}  // easy initialisation macro

// Append a string to a buffer
void abAppend(struct AppBuffer *ab, const char *s, int len);
void abFree(struct AppBuffer *ab);

#ifdef __cplusplus
}
#endif
#endif