CC = gcc
CFLAGS = -Wall -Wextra -O3
LDFLAGS = -lxcb

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

TARGET = nekowm
BUILD_DIR = build
SRC = src/nekowm.c src/util.c src/window.c
BIN = $(BUILD_DIR)/$(TARGET)

all: $(BUILD_DIR) $(BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

install: $(BIN)
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BIN) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
