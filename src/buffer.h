#ifndef EDITOR_BUFFER_H
#define EDITOR_BUFFER_H


#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUF_INIT {NULL, 0, 0}  // practical initialisation macro

#define MAX_LINE_LENGHT 1000
#define MAX_LINE_COUNT  100000


// Basic character buffer with memory mangement capabilities.
struct CharBuffer {
    char *buf;
    size_t len;
    size_t capacity;
};

// File buffer, used to store the state of an edited file
struct FileBuffer {
    struct CharBuffer *lines;
    size_t len;    // Line count
    size_t capacity;

    unsigned int file_id; // ID given by the file manager, it will be useful when it comes to save the file
};


// TODO: Comments
int CharBuffer_init(struct CharBuffer* char_buffer);
// Note this function assumes that the buffer is already initialized
int CharBuffer_grow(struct CharBuffer* char_buffer, size_t needed);

// Add text at the end of a preallocated CharBuffer
int CharBuffer_append_text(struct CharBuffer *ab, const char *s, size_t len);

// Deallocate the internal character buffer, then sets the lenght to 0.
// Note that if you used an externally/stack managed buffer to as internal buffer,
// calling this function may cause a double free and a segfault.
void CharBuffer_free(struct CharBuffer *line_buffer);


// TODO: Comments
int FileBuffer_init(struct FileBuffer* char_buffer);
// Note this function assumes that the buffer is already initialized
int FileBuffer_grow(struct FileBuffer* char_buffer, size_t needed);

// Add an empty line at the end of a file buffer
int FileBuffer_add_line(struct FileBuffer *file_buffer);

/* 
Add text to a file buffer, appending new lines accordingly everytime a new line is encountered.
If the buffer was empty, a new line will be added to store the text, otherwise, the last line is used.

Returns a status code depending on the succes of the operation.
*/
int FileBuffer_append_text(struct FileBuffer *file_buffer, const char *s);

// Deallocate each line buffer of the file, then deallocate the array.
// Consider looking `LineBuffer_free` in case you use external/stack managed 
// buffers as internal buffers as it might cause issues.
void FileBuffer_free(struct FileBuffer* file_buffer);


#ifdef __cplusplus
}
#endif
#endif