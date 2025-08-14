# Restly 

**WORK BETTER, FEEL BETTER**


**Restly gently helps you solve all your eyecare needs while using your favourite device(will soon expand to other platforms)**
**Restly helps you reduce eye strain, stay focused, and keep your energy up. No more headaches, no more deteriorating eyesignt, no more thickening glasses.**

Restly runs silently in the background and sends gentle popup notifications to remind you to take breaks, blink, and perform eye exercises based on the 20-20-20 rule and follow best ergonomic practices for your health. Moreover you can setup your own customized notifications! 

## ✨ Features

- 👁️ **Built-in eye care routine** - Guided eye exercises and neck stretches
- 🎨 **Beautiful notifications** - Elegant, non-intrusive popup design
- 🚀 **Auto-start** - Automatically starts with your system
- 🎯 **Custom messages** - Create your own popup messages
- ⏰ **Active hours** - Only show popups during specified time periods
- 🕐 **Customizable intervals** - Set break popups at your preferred frequency
- 🔧 **Swift operation** - Runs quietly in the background. Easy to start, configure and stop

## 📦 Installation

### Prerequisites

You'll need GTK+3 development libraries installed:

```bash
# Ubuntu/Debian
sudo apt install libgtk-3-dev

# Fedora
sudo dnf install gtk3-devel

# Arch Linux
sudo pacman -S gtk3
```

### Quick Install

1. Clone the repository:
```bash
git clone https://github.com/krednie/restly.git
cd restly
```

2. Run the installer:
```bash
chmod +x install.sh
./install.sh
```

The installer will:
- Compile the application
- Install it to `~/.local/bin/`
- Add the directory to your PATH
- Set up autostart configuration
- Optionally start the daemon immediately

## 🚀 Usage

### Basic Usage

```bash
# Start with default settings (20-minute intervals, eye care routine)
restly

# Custom interval (30 minutes)
restly --interval 30

# Custom message instead of eye care routine
restly --message "Time for a coffee break!" --eyecare 0

# Active during work hours only
restly --active-hours 09:00-17:00
```

### Command Line Options

| Option | Short | Description | Default |
|--------|-------|-------------|---------|
| `--interval` | `-i` | Break interval in minutes | `20` |
| `--duration` | `-d` | Popup duration in seconds | `20` |
| `--message` | `-m` | Custom popup message | `"Time to rest your eyes! (if eyecare disabled)"` |
| `--eyecare` | `-e` | Enable eye care routine (1) or custom message (0) | `1` |
| `--active-hours` | | Active time range (e.g., `09:00-17:00`) | `00:00-23:59` |
| `--stop` | | Stop the running daemon | |

### Eye Care Routine

When `--eyecare 1` is enabled, Restly guides you through a quick and easy yet cruicial routine.

This routine is based on the **20-20-20 rule**: Every 20 minutes, look at something 20 feet away for 20 seconds.

### Managing the Daemon

```bash
# Start the daemon
restly --interval 25 --eyecare 1

# Stop the daemon
restly --stop

# Check if running
pgrep restly
```

## 📁 File Structure

```
restly/
├── main.c          # Application entry point
├── config.c/.h     # Command-line argument parsing
├── daemon.c/.h     # Background daemon functionality
├── timer.c/.h      # Timer and scheduling logic
├── popup.c/.h      # GTK popup notifications
├── install.sh      # Installation script
└── README.md       # This file
```

## 🛠️ Development

### Building Manually

```bash
gcc -o restly main.c config.c daemon.c timer.c popup.c $(pkg-config --cflags --libs gtk+-3.0)
```

### Debugging

To run in foreground mode for debugging, comment out the `daemonize()` call in `main.c`.

## 🔧 Configuration Examples

### For Developers
```bash
# Short 15-minute breaks during coding hours
restly --interval 15 --active-hours 09:00-18:00 --eyecare 1
```

### For Office Workers
```bash
# Standard breaks with custom message
restly --interval 30 --message "Stand up and stretch!" --eyecare 0
```

### For Students
```bash
# Frequent popups during study sessions
restly --interval 25 --active-hours 08:00-22:00 --eyecare 1
```

## 🚨 Troubleshooting

### Common Issues

**"GTK+3 development libraries not found"**
- Install the development packages for your distribution (see Prerequisites)

**"Daemon not starting"**
- Check if another instance is running: `pgrep restly`
- Stop existing daemon: `restly --stop`

**"Popups not appearing"**
- Ensure your desktop environment supports notifications
- Check if notification permissions are enabled

**"PATH not found after installation"**
- Restart your terminal or run: `source ~/.bashrc`
- Manually add to PATH: `export PATH="$HOME/.local/bin:$PATH"`

### Uninstallation

```bash
# Stop the daemon
restly --stop

# Remove files
rm ~/.local/bin/restly
rm ~/.config/autostart/restly.desktop
rm /tmp/restlychild.pid

# Remove PATH entry from ~/.bashrc (manual)
```

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

### Development Setup

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by the 20-20-20 rule for eye health
- Built with GTK3 for native Linux integration
- Thanks to all future contributors and users providing feedback in advance. You guys are awesome.

## 💡 Tips for Healthy Computer Usage

- **Follow the 20-20-20 rule**: Every 20 minutes, look 20 feet away for 20 seconds
- **Blink frequently**: Computer work reduces blink rate by up to 60%
- **Adjust screen brightness**: Match your display to surrounding lighting
- **Position your screen**: 20-24 inches away, top at or below eye level
- **Take regular breaks**: Stand, stretch, and move around
- **Stay hydrated**: Keep water nearby and drink regularly

---

**Made with ❤️ for healthier computing**
***KREDNIE***
