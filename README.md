"I'm too lazy to open todo app/widget and mark done todo"

# DOIT

A minimalist Windows terminal-based TODO manager designed for lazy programmers who prefer working in the command line. The program automatically runs once per day during your first terminal session, helping you manage daily tasks without disrupting your workflow.

https://github.com/user-attachments/assets/5d84bfaa-ca04-492f-be8b-76d0c02a4df4

## Features

- **Ultra Lightweight Executable**
  - machine code and C program
  - This will not make your terminal bloated

- **Automatic Daily Management**
  - Runs automatically on first terminal session of the day
  - Cleans up previous day's tasks automatically
  - Can be manually triggered when needed

- **Task Persistence**
  - Tasks are saved between sessions
  - Option to carry over specific tasks to the next day
  - Automatic cleanup of completed tasks

- **Simple Interface**
  - Minimal command-line interface
  - Quick task entry and management
  - Clear task visibility

## Installation

1. Download from the releases

OR

1. Clone or download the source code
2. Compile the program:
```bash
gcc -o todo todo.c
```
3. Move the executable to a directory in your PATH or create an alias

## Usage

### Basic Commands

```bash
# Regular usage - will only show menu on first run of the day
todo

# Force show menu even if already run today
todo ls
```

### Menu Options

1. **Add new task**: Enter a new task for today
2. **Mark task for tomorrow**: Select a task to carry over to next day
3. **Exit**: Close the program

### Task Management

- By default, all tasks are considered completed at the end of each day
- Tasks must be explicitly marked "Keep for Tomorrow" to appear the next day
- New tasks can be added at any time using the `ls`

## Data Storage

The program stores its data in:
```
%APPDATA%\doit\tasks.dat
```

## Integration Tips: Make it Automatically Run on First Session

### CMD Integration
Add to your PATH:
```batch
set PATH=%PATH%;C:\path\to\todo
```

### [Clink](https://github.com/chrisant996/clink) Integration
Add an alias in your Clink configuration:
```lua
load(io.popen('todo'):read("*a"))
```

## TODO

-  Fixing bug that the first session run doesn't working with `stdout`, making first terminal session only block process without giving menu and today todo(s) feedback. This typically happen with Clink integration
