# Femto

> A lightweight, terminal-based text editor inspired by Nano, written in C.

That's what I would write if I wanned to sound professional.
Let's be clear on this one: you do NOT want to use this editor as your daily editor.
This is a fun project I'm doing. But if you want to contribute, maybe we can turn it into an actually functionning editor.

## Features

- **Minimal and Fast**: Lightweight C implementation with minimal dependencies. (Translation: memory management was made by me, do whatever you want with that info)
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

## Usage

### Opening Files

```bash
# Create or open a new file
femto filename.txt

# Open without arguments to start with empty buffer
# Note: This feature is not supported yet, even though it would
literally take me two minutes to do it. Maybe in the next commmit, maybe not.
femto
```

### Basic Navigation

- **Arrow Keys**: Move cursor up, down, left, right
- **Home**: Move to start of line
- **End**: Move to end of line
- **Page Up/Down**: Scroll through document

Notes:
Those features were more difficult to implement than I thought. Like, how ?
- You can go to the next line from the end of the current one.
- You can go to the previous line from the start of the current one. (I know this blows you mind)
- You can delete and add lines.

### Editing

- **Regular Keys**: Type to insert text
- **Backspace**: Delete character before cursor
- **Delete**: Delete character at cursor
- **Tab**: Insert tab character (configurable width (*you have to recompile the whole thing everytime*))

### File Operations

- **Ctrl+S**: Save file
- **Ctrl+Q**: Exit editor (NO PROMPT TO SAVE YET !!)

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
- Supports text operations (insert, delete, etc.) and memory management

## Version

Current version: **0.0.1** (Alpha)

This is an early-stage prototype. API and features are subject to change.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

See the LICENSE file for details.

## Future Enhancements
- [ ] File system errors handling
- [ ] Multiple file support/tabs
- [ ] Undo/Redo functionality
- [ ] Search and replace
- [ ] Configuration file support
- [ ] Mouse support (maybe some day)
- [ ] Line wrapping options
- [ ] Performance optimizations for large files
