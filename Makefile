CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lxcb

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

TARGET = nekowm
SRC = src/nekowm.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(TARGET) $(DESTDIR)$(BINDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
