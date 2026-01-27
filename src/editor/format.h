#ifndef EDITOR_FORMAT_H
#define EDITOR_FORMAT_H

#define TABULATION_SIZE 4

/*
editor/format.h

This file defines primitives for handling the formating of text by the editor.h
*/

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


// This function converts a raw text buffer into a formated buffer with proper tabulations.
// If `dest` is provided and non null, the given uffer will be overwriten with the formated text.
// Otherwise, a new buffer is allocated on the heap.
// In any case, the funtion returns a pointer to the formated text buffer. A NULL value means that an error occured.
// NOTE: This function assumes the provided buffer represent a single line. It will not behave well if the buffer contains a '\r' or '\n' character.
struct CharBuffer* format_raw_text(const struct CharBuffer *raw, struct CharBuffer *dest);


#ifdef __cplusplus
}
#endif

#endif