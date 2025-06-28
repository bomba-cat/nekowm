CC = gcc
CFLAGS = -Wall -Wextra -O3
LDFLAGS = -lxcb -lxcb-keysyms

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

TARGET = nekowm
BUILD_DIR = build
SRC = src/keyboard.c src/window.c src/event.c src/util.c src/neko.c
BIN = $(BUILD_DIR)/$(TARGET)

.PHONY: all clean install uninstall session

all: $(BUILD_DIR) $(BIN)

$(BUILD_DIR):
	@echo "Creating build/ directory"
	@mkdir -p $(BUILD_DIR)

$(BIN): $(SRC)
	@echo "Compiling src: "$^
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@echo "Removing build/ directory"
	@rm -rf $(BUILD_DIR)

install: all
	@echo "Installing into "$(DESTDIR)$(BINDIR)
	@mkdir -p $(DESTDIR)$(BINDIR)
	@cp $(BIN) $(DESTDIR)$(BINDIR)/$(TARGET)

session:
	@echo "Installing NekoWM session file to /usr/share/xsessions"
	@mkdir -p /usr/share/xsessions
	@echo "[Desktop Entry]" > /usr/share/xsessions/nekowm.desktop
	@echo "Name=NekoWM" >> /usr/share/xsessions/nekowm.desktop
	@echo "Comment=Minimal tiling window manager" >> /usr/share/xsessions/nekowm.desktop
	@echo "Exec=$(BINDIR)/$(TARGET)" >> /usr/share/xsessions/nekowm.desktop
	@echo "TryExec=$(BINDIR)/$(TARGET)" >> /usr/share/xsessions/nekowm.desktop
	@echo "Type=Application" >> /usr/share/xsessions/nekowm.desktop
	@echo "DesktopNames=nekowm" >> /usr/share/xsessions/nekowm.desktop

uninstall:
	@echo "Uninstalling "$(DESTDIR)$(BINDIR)/$(TARGET)
	@rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
