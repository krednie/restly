#!/usr/bin/env bash
set -Eeuo pipefail
IFS=$'\n\t'

# Colors
bold="$(tput bold 2>/dev/null || true)"
normal="$(tput sgr0 2>/dev/null || true)"
check() { printf "[%sOK%s] %s\n" "$bold" "$normal" "$1"; }
info()  { printf "[*] %s\n" "$1"; }
warn()  { printf "[!] %s\n" "$1"; }

script_dir="$(cd -- "$(dirname -- "$0")" && pwd -P)"
cd "$script_dir"

app_name="Restly Eye Care"
binary_name="restly"
install_bin_dir="$HOME/.local/bin"
install_bin_path="$install_bin_dir/$binary_name"
wrapper_name="restly-start"
wrapper_path="$install_bin_dir/$wrapper_name"
autostart_dir="$HOME/.config/autostart"
autostart_file="$autostart_dir/restly.desktop"
systemd_user_dir="$HOME/.config/systemd/user"
systemd_unit="$systemd_user_dir/restly.service"

printf "%s\n" "Installing $app_name …"

# --- Prerequisites ---------------------------------------------------------
need_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    warn "Required tool '$1' not found."
    return 1
  fi
}

need_pkg() {
  if ! pkg-config --exists "$1"; then
    return 1
  fi
}

missing=0
need_cmd gcc || missing=1
need_cmd pkg-config || missing=1

if ! need_pkg gtk+-3.0; then
  warn "GTK+3 development libraries not found."
  printf "\nInstall them with your distro package manager:\n"
  printf "  Ubuntu/Debian: sudo apt install libgtk-3-dev\n"
  printf "  Fedora:        sudo dnf install gtk3-devel\n"
  printf "  Arch:          sudo pacman -S gtk3\n\n"
  exit 1
fi

if (( missing )); then
  warn "Please install missing tools (gcc, pkg-config) and re-run."
  exit 1
fi
check "Build prerequisites present"

# --- Build -----------------------------------------------------------------
CFLAGS_DEFAULT="-O2 -Wall -Wextra"
RESPECT_ENV_FLAGS=${RESPECT_ENV_FLAGS:-0}
VERBOSE=${VERBOSE:-0}

# Allow overriding compiler; prefer system gcc if not provided
if [[ -n "${CC:-}" ]]; then
  cc_bin="$CC"
else
  if [[ -x "/usr/bin/gcc" ]]; then
    cc_bin="/usr/bin/gcc"
  else
    cc_bin="$(command -v gcc || true)"
  fi
fi

# Sanitize helper: remove standalone -O tokens
sanitize_flags() {
  local input_flags=("$@")
  local out=()
  for f in "${input_flags[@]}"; do
    if [[ "$f" == "-O" ]]; then
      continue
    fi
    out+=("$f")
  done
  printf '%s\n' "${out[@]}"
}

if [[ "$RESPECT_ENV_FLAGS" == "1" ]]; then
  # Use env CFLAGS if set, otherwise default
  read -r -a _cflags_array <<< "${CFLAGS:-$CFLAGS_DEFAULT}"
else
  read -r -a _cflags_array <<< "$CFLAGS_DEFAULT"
fi

# Always sanitize
mapfile -t sanitized_cflags < <(sanitize_flags "${_cflags_array[@]}")

info "Compiling $binary_name …"

if [[ -z "$cc_bin" || ! -x "$cc_bin" ]]; then
  warn "gcc not found in PATH"
  exit 1
fi

# Collect gtk flags into an array to preserve word boundaries and sanitize too
gtk_flags=()
if pkg_output="$(pkg-config --cflags --libs gtk+-3.0)"; then
  read -r -a _gtk_tmp <<< "$pkg_output"
  mapfile -t gtk_flags < <(sanitize_flags "${_gtk_tmp[@]}")
else
  warn "pkg-config failed to return gtk+-3.0 flags"
  exit 1
fi

if [[ "$VERBOSE" == "1" ]]; then
  printf "Using compiler: %s\n" "$cc_bin"
  printf "Sanitized CFLAGS: %q " "${sanitized_cflags[@]}"; printf "\n"
  printf "GTK flags: %q " "${gtk_flags[@]}"; printf "\n"
fi

# Invoke the compiler with a minimal environment to neutralize wrappers reading env flags
env -u CFLAGS -u CPPFLAGS -u CXXFLAGS -u LDFLAGS \
  "$cc_bin" "${sanitized_cflags[@]}" \
  -o "$binary_name" \
  main.c config.c daemon.c timer.c popup.c \
  "${gtk_flags[@]}"

check "Compilation successful"

# --- Install ---------------------------------------------------------------
mkdir -p "$install_bin_dir"
cp -f "$binary_name" "$install_bin_path"
chmod +x "$install_bin_path"
check "Installed to $install_bin_path"

# Ensure ~/.local/bin is on PATH for login shells and interactive shells
ensure_path_line='export PATH="$HOME/.local/bin:$PATH"'
append_line_if_missing() {
  local file="$1"
  local line="$2"
  if [ -f "$file" ]; then
    if ! grep -Fqx "$line" "$file"; then
      printf "\n%s\n" "$line" >> "$file"
      check "Added ~/.local/bin to PATH in ${file/$HOME/~}"
    fi
  fi
}

# Put in .profile for GUI sessions, and .bashrc for interactive shells
append_line_if_missing "$HOME/.profile" "$ensure_path_line" || true
append_line_if_missing "$HOME/.bashrc"  "$ensure_path_line" || true

# --- Interactive command setup --------------------------------------------
printf "\nLet's configure your default $binary_name command. Press Enter to accept defaults.\n\n"
read -r -p "Interval minutes [20]: " interval
interval=${interval:-20}
if ! [[ $interval =~ ^[0-9]+$ ]]; then warn "Invalid interval; using 20"; interval=20; fi

read -r -p "Popup duration seconds [20]: " duration
duration=${duration:-20}
if ! [[ $duration =~ ^[0-9]+$ ]]; then warn "Invalid duration; using 20"; duration=20; fi

read -r -p "Enable eye care routine? [Y/n]: " eyecare_ans
case "${eyecare_ans,,}" in
  n|no) eyecare=0 ;;
  *)    eyecare=1 ;;
esac

message_default="Time to rest your eyes!"
message="$message_default"
if [ "$eyecare" -eq 0 ]; then
  read -r -p "Custom message [${message_default}]: " msg
  message=${msg:-$message_default}
fi

read -r -p "Active hours [00:00-23:59]: " active_hours
active_hours=${active_hours:-00:00-23:59}

# Build args array
args=(
  --interval "$interval"
  --duration "$duration"
  --eyecare "$eyecare"
  --active-hours "$active_hours"
)
if [ "$eyecare" -eq 0 ]; then
  args+=(--message "$message")
fi

# Create wrapper script so .desktop can use an absolute path without fragile quoting
mkdir -p "$install_bin_dir"
if [ -f "$wrapper_path" ]; then
  cp -f "$wrapper_path" "${wrapper_path}.bak" || true
fi

escaped_cmd=$(printf '%q ' "$install_bin_path" "${args[@]}")
cat > "$wrapper_path" <<EOF
#!/usr/bin/env bash
set -Eeuo pipefail
exec $escaped_cmd
EOF
chmod +x "$wrapper_path"
check "Created launcher $wrapper_path"

# --- Optional systemd user service ----------------------------------------
systemd_ok=0
if command -v systemctl >/dev/null 2>&1 && systemctl --user --version >/dev/null 2>&1; then
  mkdir -p "$systemd_user_dir"
  cat > "$systemd_unit" <<EOF
[Unit]
Description=$app_name
After=default.target

[Service]
Type=simple
ExecStart=$wrapper_path
Restart=on-failure
RestartSec=3

[Install]
WantedBy=default.target
EOF
  if systemctl --user daemon-reload >/dev/null 2>&1 && \
     systemctl --user enable --now restly.service >/dev/null 2>&1; then
    check "Enabled systemd user service (restly.service)"
    systemd_ok=1
  else
    warn "Could not enable systemd user service. Falling back to desktop autostart."
  fi
else
  info "systemd user not available; using desktop autostart."
fi

# --- Autostart (.desktop) --------------------------------------------------
mkdir -p "$autostart_dir"
if [ -f "$autostart_file" ]; then
  cp -f "$autostart_file" "${autostart_file}.bak" || true
fi

cat > "$autostart_file" <<EOF
[Desktop Entry]
Type=Application
Name=$app_name
Comment=Eye care reminder daemon
Exec=$wrapper_path
Icon=applications-utilities
StartupNotify=false
NoDisplay=true
Hidden=false
X-GNOME-Autostart-enabled=true
# Common DEs; harmless if ignored by others
OnlyShowIn=GNOME;KDE;XFCE;LXDE;LXQt;Unity;
X-GNOME-Autostart-Delay=5
EOF
# If systemd service is enabled, hide the .desktop to avoid duplicate starts
if [ "$systemd_ok" -eq 1 ]; then
  sed -i 's/^Hidden=false/Hidden=true/' "$autostart_file" || true
  check "Autostart .desktop created (hidden, managed by systemd)"
else
  check "Autostart entry created at ${autostart_file/$HOME/~}"
fi

# --- Offer to start now ----------------------------------------------------
printf "\nYour configured command is:\n  %s\n" "$install_bin_path ${args[*]}"

read -r -p "Start $binary_name now (and restart if already running)? [Y/n]: " start_now
case "${start_now,,}" in
  n|no)
    ;;
  *)
    if pgrep -x "$binary_name" >/dev/null 2>&1; then
      info "$binary_name is running; restarting with new settings…"
      # Try to stop via CLI if supported; otherwise kill by pidfile
      "$install_bin_path" --stop >/dev/null 2>&1 || true
      sleep 0.5 || true
      if pgrep -x "$binary_name" >/dev/null 2>&1; then
        pkill -x "$binary_name" || true
        sleep 0.2 || true
      fi
    fi
    nohup "$wrapper_path" >/dev/null 2>&1 &
    check "$binary_name started"
    ;;
 esac

printf "\nInstallation complete!\n"
printf "- Binary:         %s\n" "${install_bin_path/$HOME/~}"
printf "- Launcher:       %s\n" "${wrapper_path/$HOME/~}"
if [ "$systemd_ok" -eq 1 ]; then
  printf "- systemd unit:   %s\n" "${systemd_unit/$HOME/~}"
fi
printf "- Autostart file: %s\n" "${autostart_file/$HOME/~}"
printf "\n$binary_name will start automatically when you log in.\n"
printf "Edit settings later by re-running this installer or editing '%s'.\n" "${wrapper_path/$HOME/~}"