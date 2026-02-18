# Femto

A lightweight, terminal-based text editor inspired by Nano, written in C.

## Features

- **Minimal and Fast**: Lightweight C implementation with minimal dependencies
- **Nano-inspired**: Familiar keybindings and interface reminiscent of Nano editor
- **Terminal-based**: Full TUI (Text User Interface) for editing files in the terminal
- **Cross-platform**: Runs on Unix-like systems (Linux, macOS, BSD)

## Getting Started

### Prerequisites

- GCC compiler
- Make
- POSIX-compliant terminal
- Linux, macOS, or BSD system

### Building

```bash
# Build in debug mode (default)
make

# Build in release mode (optimized)
make release

# Run the editor
make run
```

### Installation

```bash
# Build the project
make release

# The binary will be available at bin/femto
./bin/femto [filename]
```

## Usage

### Opening Files

```bash
# Create or open a new file
femto filename.txt

# Open without arguments to start with empty buffer
femto
```

### Basic Navigation

- **Arrow Keys**: Move cursor up, down, left, right
- **Ctrl+Home**: Move to start of file
- **Ctrl+End**: Move to end of file
- **Page Up/Down**: Scroll through document

### Editing

- **Regular Keys**: Type to insert text
- **Backspace**: Delete character before cursor
- **Delete**: Delete character at cursor
- **Tab**: Insert tab character (configurable width)

### File Operations

- **Ctrl+S**: Save file
- **Ctrl+O**: Open file
- **Ctrl+X**: Exit editor (prompts to save if unsaved changes)

### Other Commands

- **Ctrl+G**: Go to line
- **Ctrl+F**: Find text
- **Ctrl+H**: Find and replace

## Project Structure

```
femto/
├── src/
│   ├── main.c           # Entry point and initialization
│   ├── editor.h         # Editor configuration and interface
│   ├── editor.c         # Core editor logic
│   ├── terminal.h       # Terminal handling interface
│   ├── terminal.c       # Terminal mode configuration
│   ├── global.h         # Global utilities and constants
│   └── editor/          # Editor subsystems
│       ├── buffer.h     # Text buffer management
│       ├── input.h      # Input processing
│       └── format.h     # Text formatting and display
├── Makefile             # Build configuration
└── LICENSE              # License information
```

## Architecture

### Core Components

**Editor** (`editor.h`, `editor.c`)
- Manages editor state (cursor position, viewport, etc.)
- Handles screen rendering and refresh cycles
- Processes user input
- Manages file operations

**Terminal** (`terminal.h`, `terminal.c`)
- Enables raw mode for character-by-character input
- Handles terminal size and cursor positioning
- Manages terminal state restoration

**Buffer** (in `editor/`)
- Manages file content and formatting
- Handles character and line buffers
- Supports text operations (insert, delete, etc.)

### Key Design Patterns

- **State Management**: `EditorConfig` struct maintains editor state
- **Double Buffering**: Uses character buffers for efficient rendering
- **Signal Handling**: Proper cleanup on exit via `atexit()`
- **Raw Mode**: Terminal operates in raw mode for direct control

## Version

Current version: **0.0.1** (Alpha)

This is an early-stage prototype. API and features are subject to change.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

See the LICENSE file for details.

## Future Enhancements

- [ ] Syntax highlighting
- [ ] Multiple file support/tabs
- [ ] Undo/Redo functionality
- [ ] Search and replace
- [ ] Configuration file support
- [ ] Mouse support
- [ ] Line wrapping options
- [ ] Performance optimizations for large files
