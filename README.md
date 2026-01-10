A complete CRUD contact management application built from scratch in C.

## 🏗️ Architecture
```input.c/.h``` 
Industrial-strength input validation library (22/22 tests passing)

```contact.c/.h```
Core contact management logic with search/match

```main.c```
User interface driver with full CRUD operations





## ✨ Features
RFC 5321-compliant email validation - Not just regex, actual spec compliance

International phone validation - Supports 20+ country formats

Fuzzy search - Find contacts even with typos

Layered architecture - Separation of concerns enforced

Git-powered development - Every commit tells part of the story






## 🚀 Installation
```bash```
git clone https://github.com/Lord-Bolt/contact-manager.git
cd contact-manager
gcc main.c contact.c input.c -o contact_manager.exe -Wall -Wextra -std=c99
./contact_manager.exe





## 🔮 Roadmap
v1.2.0: File I/O persistence (JSON/CSV)

v1.3.0: Database engine prototype

v2.0.0: HTTP server interface
