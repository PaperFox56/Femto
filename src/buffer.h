#ifndef EDITOR_BUFFER_H
#define EDITOR_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#define BUF_INIT {NULL, 0}  // practical initialisation macro

#define MAX_LINE_LENGHT 1000
#define MAX_LINE_COUNT  100000

/// A buffer to which you can append text (yeah that's never been done before)
struct AppBuffer {
    char *buf;
    unsigned int len;
};

// Append a string to a buffer
void abAppend(struct AppBuffer *ab, const char *s, int len);
void abFree(struct AppBuffer *ab);


// Basic character buffer representing a single line.
struct LineBuffer {
    char *buf;
    unsigned int len;
};

// File buffer, used to store the state of an edited file
struct FileBuffer {
    struct LineBuffer *lines;
    unsigned int len;    // Line count

    unsigned int file_id; // ID given by the file manager, it will be useful when it comes to save the file
};


// Deallocate the internal character buffer, then sets the lenght to 0.
// Note that if you used an externally/stack managed buffer to as internal buffer,
// calling this function may cause a double free and a segfault.
void LineBuffer_free(struct LineBuffer *line_buffer);

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