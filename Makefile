CC = gcc
CFLAGS = -Wall -Wextra -std=c99

cm.exe:
	$(CC) $(CFLAGS) main.c contact_dynamic.c contact_file.c input.c sqlite3.c contact_db.c contact_storage.c -o cm.exe

clean:
	del /f /q cm.exe *.o

.PHONY: clean