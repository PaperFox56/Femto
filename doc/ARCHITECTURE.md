# Femto Architecture Documentation

## Overview

Femto is a terminal-based text editor written in C, inspired by the Nano editor. This document provides an in-depth look at the architecture, design decisions, and implementation details.

## System Architecture

```
┌─────────────────────────────────────────────────────┐
│                    Main Loop                        │
│  while(1) { refresh_screen(); process_keypress(); }│
└────────────┬────────────────────────────────────────┘
             │
    ┌────────┴──────────┐
    │                   │
    v                   v
┌─────────────┐   ┌──────────────────┐
│   Editor    │   │  Terminal I/O    │
│  - Render   │   │  - Raw Mode      │
│  - State    │   │  - Input Read    │
│  - Scroll   │   │  - Cursor Pos    │
└──────┬──────┘   └──────────────────┘
       │
       v
┌─────────────────────────────────────┐
│       File Buffer                   │
│  - Character buffers                │
│  - Line management                  │
│  - Text operations                  │
└─────────────────────────────────────┘
```

## Key Components

### 1. Main Entry Point (`src/main.c`)

**Responsibilities:**
- Program initialization
- Signal and error handling
- Main event loop setup
- Cleanup on exit

**Key Functions:**
- `main()`: Entry point
- `panic()`: Error handling with cleanup
- `print_log()`: Debug logging to file
- `clean_and_exit()`: Shutdown handler

**Event Loop:**
```c
while (1) {
    editor_refresh_screen();
    editor_process_keypress();
}
```

### 2. Editor Core (`src/editor.h`, `src/editor.c`)

**Data Structure - EditorConfig:**
```c
struct EditorConfig {
    int cx, cy;              // Cursor position (formatted)
    int rx;                  // Cursor position (raw)
    int rows_offset;         // Scroll offset - rows
    int cols_offset;         // Scroll offset - columns
    int margins;             // Line number margin width
    int screen_rows;         // Terminal height
    int screen_cols;         // Terminal width
    int rows, cols;          // Editable area dimensions
    struct CharBuffer message;
    time_t message_timeout;
};
```

**State Management:**
- Single global `EditorConfig` instance maintains editor state
- Separate cursor tracking for formatted (display) and raw (file) positions
- Viewport management with scroll offsets

**Key Functions:**

| Function | Purpose |
|----------|---------|
| `editor_init()` | Initialize editor, get terminal size, setup buffers |
| `editor_refresh_screen()` | Main rendering function - updates entire display |
| `editor_scroll()` | Adjusts viewport to keep cursor visible |
| `editor_draw_rows()` | Renders file content with line numbers |
| `editor_draw_status_line()` | Displays status bar with file info |
| `editor_show_message()` | Displays status messages |
| `editor_process_keypress()` | Reads and handles user input |
| `editor_open_file()` | Opens and loads file into buffer |
| `editor_on_exit()` | Cleanup handler |
| `editor_set_message()` | Sets status message with formatting |

**Cursor Position Management:**

The editor maintains two cursor positions:
- **cx/cy**: Display position (formatted) - accounts for tab expansion
- **rx**: Raw position (file) - actual position in file buffer

Helper functions convert between them:
```c
int calculate_rx_from_cx()  // Format -> Raw
int calculate_cx_from_rx()  // Raw -> Format
```

This dual-position system handles tab characters which render as multiple visual columns but occupy single bytes in the file.

### 3. Terminal Handler (`src/terminal.h`, `src/terminal.c`)

**Responsibilities:**
- Terminal mode management
- Input reading
- Cursor and window control
- Terminal state preservation

**Key Concepts:**

**Raw Mode:**
- Disables line buffering
- Disables echo (characters not printed automatically)
- Disables signal processing (Ctrl+C, Ctrl+Z disabled)
- Enables direct character-by-character input

**Terminal Flags Modified:**
```c
termios_flags.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
termios_flags.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
termios_flags.c_cflag |= CS8;
termios_flags.c_oflag &= ~OPOST;
```

**Key Functions:**

| Function | Purpose |
|----------|---------|
| `enable_raw_mode()` | Switch terminal to raw mode, save original state |
| `disable_raw_mode()` | Restore original terminal settings |
| `getWindowSize()` | Query terminal for current dimensions |
| `getCursorPosition()` | Query cursor position using ANSI escape sequences |

**Escape Sequences Used:**
```
\x1b[2J        - Clear entire screen
\x1b[H         - Move cursor to home (0,0)
\x1b[6n        - Query cursor position (DSR)
\x1b[?25h/l    - Show/hide cursor
```

### 4. Buffer System (`src/editor/buffer.h`)

**Data Structure - CharBuffer:**
```c
struct CharBuffer {
    char *buf;        // Character array
    size_t capacity;  // Allocated size
    size_t len;       // Current length
};
```

**Data Structure - FileBuffer:**
```c
struct FileBuffer {
    char *filename;           // File path
    struct CharBuffer *raw;   // Raw file lines
    struct CharBuffer *fmt;   // Formatted display lines
    size_t len;               // Number of lines
    size_t capacity;          // Allocated line capacity
    struct CharBuffer raw_buf;
    struct CharBuffer fmt_buf;
};
```

**Responsibilities:**
- Character and line buffer management
- Memory allocation and growth
- Text insertion and deletion
- Format conversion (tab expansion)

## Data Flow

### File Loading
```
1. user runs: femto filename.txt
2. main() calls editor_open_file("filename.txt")
3. File contents loaded into FileBuffer.raw
4. Raw lines converted to formatted display lines (tabs expanded)
5. editor_init() initializes viewport and state
```

### Text Rendering
```
1. editor_refresh_screen() called each iteration
2. editor_scroll() adjusts viewport to show cursor
3. clear_screen() and reset_cursor_position() via ANSI codes
4. editor_draw_rows() renders visible portion of file
   - Each line prefixed with line number
   - Tab characters expanded to TABULATION_SIZE spaces
5. editor_draw_status_line() draws status bar
6. editor_show_message() displays any status messages
7. Cursor positioned at current cx,cy location
```

### Input Processing
```
1. editor_process_keypress() reads one character
2. Character matched against keybindings
3. Corresponding action executed (move cursor, insert char, etc.)
4. State updated
5. Next iteration calls refresh_screen()
```

## Design Decisions

### 1. Global State vs. Passed Parameters
**Decision**: Use global `EditorConfig` struct
**Rationale**: 
- Single editor instance simplifies API
- Matches typical text editor architecture
- Future enhancement: support multiple editor instances

### 2. Dual Cursor Positions
**Decision**: Maintain both `cx` (formatted) and `rx` (raw)
**Rationale**:
- Tab characters expand to multiple columns visually
- File storage uses single-byte tabs
- Two positions prevent constant conversion

### 3. Raw Mode Terminal Control
**Decision**: Enable POSIX raw mode
**Rationale**:
- Direct character-by-character control
- Enables implementation of vim/nano-like bindings
- Alternative would be line-buffered input

### 4. ANSI Escape Sequences
**Decision**: Use standard ANSI/VT100 escape codes
**Rationale**:
- Portable across most terminals
- Minimal dependencies
- Direct, efficient control

### 5. Single Buffer Instance
**Decision**: Global `FileBuffer` (TODO: improve)
**Rationale**:
- Current alpha stage simplification
- Future: support multiple open files/tabs

## Memory Management

**Strategy**: Simple allocation with manual cleanup

**Patterns:**
- Buffer allocation grows exponentially
- Freed in `editor_on_exit()`
- Registered via `atexit()` for safety

**Potential Issues:**
- No undo/redo buffer (future enhancement)
- No leak protection on early exit
- Fixed buffer growth strategy

## Input Handling

**Character Reading:**
```c
char c;
if (read(STDIN_FILENO, &c, 1) != 1)
    return;  // Timeout or error

if (c == CTRL('s')) { /* save */ }
else if (c == ARROW_UP) { /* move up */ }
// ... more keybindings
```

**Multi-byte Sequences:**
- Arrow keys sent as 3-byte ANSI sequences: `\x1b[A`, `\x1b[B`, etc.
- Parsed character-by-character with state tracking

## Future Architecture Improvements

1. **Plugin System**: Modular architecture for syntax highlighting
2. **Multi-file Support**: Multiple open buffers with tab management
3. **Undo/Redo**: Command buffer for reversible operations
4. **Configuration**: Settings file support
5. **Mouse Support**: Button events and scroll handling
6. **Search Index**: Efficient text search with caching
7. **Large File Support**: Lazy-loading for files > available RAM

## Building and Debugging

**Build Modes:**
- Debug: `-g` flag, full symbols
- Release: `-O2` optimization

**Debugging:**
- Log messages written to `femto.log`
- Use `make debug` to build with debug symbols
- GDB compatible: `gdb ./bin/femto`

**Performance Profiling:**
- Use `perf` for performance analysis
- Watch for excessive screen redraws
- Profile buffer operations on large files