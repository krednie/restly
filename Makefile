# Restly Eye Care - Makefile
# Compile with: make
# Clean with: make clean
# Install with: make install

CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c99
PKG_CONFIG = pkg-config
GTK_FLAGS = $(shell $(PKG_CONFIG) --cflags --libs gtk+-3.0)

# Target binary name
TARGET = restly
CONTROLLER = restly_controller.py

# Source files
SOURCES = main.c config.c daemon.c timer.c popup.c command_queue.c
OBJECTS = $(SOURCES:.c=.o)

# Installation paths
INSTALL_DIR = $(HOME)/.local/bin
AUTOSTART_DIR = $(HOME)/.config/autostart
SYSTEMD_DIR = $(HOME)/.config/systemd/user

# Default target
all: $(TARGET)

# Build the main binary
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJECTS) -o $(TARGET) $(GTK_FLAGS)
	@echo "Build complete!"

# Compile object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(shell $(PKG_CONFIG) --cflags gtk+-3.0) -c $< -o $@

# Check dependencies
deps-check:
	@echo "Checking build dependencies..."
	@command -v $(CC) >/dev/null 2>&1 || { echo "ERROR: gcc not found"; exit 1; }
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo "ERROR: pkg-config not found"; exit 1; }
	@$(PKG_CONFIG) --exists gtk+-3.0 || { echo "ERROR: GTK+3 development libraries not found"; exit 1; }
	@command -v python3 >/dev/null 2>&1 || { echo "ERROR: python3 not found"; exit 1; }
	@python3 -c "import gi; gi.require_version('Gtk', '3.0')" 2>/dev/null || { echo "ERROR: Python GTK bindings not found"; exit 1; }
	@echo "All dependencies satisfied!"

# Install the application
install: $(TARGET) deps-check
	@echo "Installing Restly..."
	@mkdir -p $(INSTALL_DIR)
	@install -m 0755 $(TARGET) $(INSTALL_DIR)/$(TARGET)
	@install -m 0755 $(CONTROLLER) $(INSTALL_DIR)/$(CONTROLLER)
	@echo "Creating launcher scripts..."
	@echo '#!/usr/bin/env bash\nset -Eeuo pipefail\nif pgrep -x "restly" >/dev/null 2>&1; then\n  exit 0\nfi\nexec "$(HOME)/.local/bin/restly" --interval 20 --duration 20 --eyecare 1 --active-hours 00:00-23:59' > $(INSTALL_DIR)/restly-start
	@echo '#!/usr/bin/env bash\nset -Eeuo pipefail\nif pgrep -f "restly_controller.py" >/dev/null 2>&1; then\n  exit 0\nfi\nexec python3 "$(HOME)/.local/bin/restly_controller.py"' > $(INSTALL_DIR)/restly-controller
	@chmod +x $(INSTALL_DIR)/restly-start $(INSTALL_DIR)/restly-controller
	@mkdir -p $(HOME)/.config/restly/commands
	@echo "Setting up autostart..."
	@mkdir -p $(AUTOSTART_DIR)
	@echo '[Desktop Entry]\nType=Application\nName=Restly Eye Care\nComment=Eye care reminder daemon\nExec=$(INSTALL_DIR)/restly-start\nIcon=applications-utilities\nStartupNotify=false\nNoDisplay=true\nHidden=false\nX-GNOME-Autostart-enabled=true\nX-GNOME-Autostart-Delay=5' > $(AUTOSTART_DIR)/restly.desktop
	@echo '[Desktop Entry]\nType=Application\nName=Restly Eye Care Controller\nComment=Restly system tray controller\nExec=$(INSTALL_DIR)/restly-controller\nIcon=preferences-system-time\nStartupNotify=false\nNoDisplay=true\nHidden=false\nX-GNOME-Autostart-enabled=true\nX-GNOME-Autostart-Delay=10' > $(AUTOSTART_DIR)/restly-controller.desktop
	@echo "Installation complete!"
	@echo "Installed to: $(INSTALL_DIR)/$(TARGET)"
	@echo "Controller:   $(INSTALL_DIR)/$(CONTROLLER)"

# Quick build and test
test: $(TARGET)
	@echo "Testing build..."
	@./$(TARGET) --help 2>/dev/null || echo "Binary compiled successfully!"

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Uninstall
uninstall:
	@echo "Uninstalling Restly..."
	@$(INSTALL_DIR)/$(TARGET) --stop 2>/dev/null || true
	@rm -f $(INSTALL_DIR)/$(TARGET)
	@rm -f $(INSTALL_DIR)/$(CONTROLLER)
	@rm -f $(INSTALL_DIR)/restly-start
	@rm -f $(INSTALL_DIR)/restly-controller
	@rm -f $(AUTOSTART_DIR)/restly.desktop
	@rm -f $(AUTOSTART_DIR)/restly-controller.desktop
	@rm -rf $(HOME)/.config/restly
	@echo "Uninstallation complete!"

# Development build (with debug symbols)
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Show help
help:
	@echo "Restly Build System"
	@echo "=================="
	@echo "Available targets:"
	@echo "  all        - Build the application (default)"
	@echo "  deps-check - Check if all dependencies are installed"
	@echo "  install    - Build and install the application"
	@echo "  test       - Build and test the binary"
	@echo "  debug      - Build with debug symbols"
	@echo "  clean      - Remove build files"
	@echo "  uninstall  - Remove installed files"
	@echo "  help       - Show this help message"

# Phony targets
.PHONY: all deps-check install test clean uninstall debug help

# Dependencies for header files
main.o: main.c timer.h daemon.h config.h
config.o: config.c config.h daemon.h
daemon.o: daemon.c daemon.h
timer.o: timer.c timer.h config.h popup.h command_queue.h
popup.o: popup.c popup.h
command_queue.o: command_queue.c command_queue.h config.h