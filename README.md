# Contact Manager

A command-line contact management tool written in pure C (C99). It stores contacts in an SQLite database, with automatic fallback to a custom binary file format featuring backup rotation and checksum verification. The project was built as a hands-on exercise in systems programming, modular design, and database integration.

## Table of Contents

- [Features](#features)
- [Building](#building)
- [Usage](#usage)
- [Architecture](#architecture)
- [File Overview](#file-overview)
- [Future Plans](#future-plans)
- [Known Issues](#known-issues)
- [License](#license)

## Features

- **Dynamic in‑memory storage** – Contacts are held in a dynamic array that grows as needed.
- **Full CRUD operations** – Add, list, search, edit, and delete contacts.
- **Input validation** – Names, phone numbers, and email addresses are validated before being accepted.
- **Hybrid persistent storage**
  - Primary: SQLite database (`contacts.db`) providing efficient queries and automatic integrity.
  - Legacy fallback: a custom binary file format (`contacts.dat`) with a three‑generation backup rotation (`.bak1`, `.bak2`, `.bak3`) and Fletcher‑32 checksums.
  - The application automatically uses the database if available; otherwise it falls back to the legacy file.
- **Explicit save\/load model** – Changes are not written to persistent storage until the user explicitly chooses “Save to File”. A “Load from File” option reloads data from storage.
- **Modular, layered architecture** – UI, business logic, and storage are cleanly separated into distinct modules.
- **Portable** – Relies only on standard C and the SQLite amalgamation. No external libraries or package managers needed.
- **Cross‑platform clear screen** – `clear_screen()` uses platform‑specific commands or a fallback sequence.

## Building

### Requirements

- GCC (tested with MinGW‑w64 on Windows; other environments should work with minor adjustments)
- SQLite amalgamation source files (`sqlite3.c` and `sqlite3.h`) – already included in the repository

### Compilation

A Makefile is provided. To build the executable:

```bash
mingw32-make
```

Alternatively, you can compile manually with:
```bash
gcc -Wall -Wextra -std=c99 main.c contact_dynamic.c contact_file.c input.c sqlite3.c contact_db.c contact_storage.c -o cm.exe
```
The output is cm.exe.
### Cleaning
```bash
mingw32-make clean
```
This removes the executable and any intermediate object files.
### Usage

Run cm.exe from the terminal. The main menu guides you through the available operations:

    Add Contact – Enter name, phone number, and email. All fields are validated.

    List All Contacts – Displays every contact, with optional sorting by ID or name.

    Search Contact – Search by ID, name, phone, or email.

    Delete Contact – Remove a contact by ID after confirmation.

    Edit Contact – Modify a contact’s name, phone, or email.

    Save to File – Persists the current contact list to both the SQLite database (if active) and the legacy binary file with backup rotation.

    Load from File – Reloads contacts from persistent storage. It tries the database first; if that fails or is empty, it falls back to the legacy file and its backups.

    Clear Screen – Clears the terminal.

    Exit – Quits the program. On exit, the program does not automatically save; you must use “Save to File” before exiting if you want to keep changes.

## Legacy file details

The legacy format uses a custom binary layout with magic numbers (LRBT / TRBL), a header containing contact count and timestamps, and a Fletcher‑32 data checksum. The backup rotation behaves as follows:

    On each save, the existing contacts.dat is renamed to contacts.dat.bak1, bak1 to bak2, and bak2 to bak3.

    If loading fails on contacts.dat, the loader tries .bak1, .bak2, and .bak3 in order.

    If a backup is successfully loaded, it is automatically promoted to the main file.

## Architecture

The code is organized into layers that hide implementation details behind clean interfaces:

    UI layer – main.c contains the menu loop and calls into the core operations.

    Business logic – contact_dynamic.c/.h implements the in‑memory contact list (a dynamic array) and all CRUD helpers, including validation.

    Storage abstraction – contact_storage.c/.h provides a unified API (storage_init, storage_load_all, storage_save_all, …). Internally, it decides whether to use the database or the legacy file based on availability.

    Database backend – contact_db.c/.h wraps all SQLite operations (open, close, create table, insert, update, delete, search).

    Legacy file backend – contact_file.c/.h handles the custom binary format, backup rotation, and checksums.

    Input utilities – input.c/.h offers safe keyboard input routines (string, integer, yes/no prompts).

## File Overview
## File Overview

| File | Role |
|:---|:---|
| `main.c` | Program entry point, menu, and UI logic |
| `contact_dynamic.c` / `.h` | In‑memory contact list (dynamic array) |
| `contact_file.c` / `.h` | Legacy binary persistence (checksums, backup rotation) |
| `contact_db.c` / `.h` | SQLite database operations |
| `contact_storage.c` / `.h` | Storage layer – selects database or legacy file |
| `input.c` / `.h` | Safe user input functions |
| `sqlite3.c` / `.h` | SQLite amalgamation (external, unmodified) |
| `Makefile` | Build script |
| `README.md` | This file |
### Future Plans

    Integrate an HTTP server to manage contacts over a REST API.

    Write a comprehensive test suite.

    Address all compiler warnings and perform a code cleanup pass.

### Known Issues

The build currently produces several harmless compiler warnings:

    Unused function parameters (e.g., in WinMain and main) – these are placeholders required by the Windows API and do not affect execution.

    A few static helper functions (fletcher32_update_stream, write_contact, calculate_packed_checksum_stream) are declared in a header but only used in their respective .c files; the declarations will be moved or removed in a cleanup.

    Some checks in contact_storage.c that compare array addresses against NULL are logically unnecessary (arrays can never be NULL). These will be simplified.

All warnings have been reviewed and are considered benign. The program operates correctly and has been tested on Windows with MinGW‑w64.

## License

This project is distributed under the **GNU General Public License v3.0**.

See the [LICENSE](LICENSE) file for the full text.

**SPDX-License-Identifier:** GPL-3.0-only


*Made with ❤️ and a lot of C by [Lord-Bolt](https://github.com/Lord-Bolt)*