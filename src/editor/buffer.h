#ifndef EDITOR_BUFFER_H
#define EDITOR_BUFFER_H


#include <stddef.h>
#include "buffer/char_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUF_INIT {NULL, 0, 0}  // practical initialisation macro

#define MAX_LINE_LENGHT 1000
#define MAX_LINE_COUNT  100000


// File buffer, used to store the state of an edited file
struct FileBuffer {
    struct CharBuffer *raw; // The actual text
    struct CharBuffer *format;  // What is rendered on the screen
    size_t len;    // Line count
    size_t capacity;

    // The full path used to open the file (relative or absolute)
    struct CharBuffer path;
    // The name of the file
    struct CharBuffer file_name;
};



// TODO: Comments
int FileBuffer_init(struct FileBuffer* char_buffer);
// Note this function assumes that the buffer is already initialized
int FileBuffer_grow(struct FileBuffer* char_buffer, size_t needed);

// Add an empty line at the end of a file buffer
int FileBuffer_add_line(struct FileBuffer *file_buffer);

// Insert `len` lines at the given index
int FileBuffer_insert_lines(struct FileBuffer *file_buffer, size_t index, size_t len);

void FileBuffer_remove_lines(struct FileBuffer *file_buffer, size_t index, size_t len);


void FileBuffer_insert_text(struct FileBuffer *file_buffer, const char *s, size_t line, size_t index, size_t maxlen);

/* 
Add text to a file buffer, appending new lines accordingly everytime a new line is encountered.
If the buffer was empty, a new line will be added to store the text, otherwise, the last line is used.

This function will panic of the oparation fails
*/
void FileBuffer_append_text(struct FileBuffer *file_buffer, const char *s, size_t maxlen);


// Deallocate each line buffer of the file, then deallocate the array.
// Consider looking `LineBuffer_free` in case you use external/stack managed 
// buffers as internal buffers as it might cause issues.
void FileBuffer_free(struct FileBuffer* file_buffer);


#ifdef __cplusplus
}
#endif
#endif