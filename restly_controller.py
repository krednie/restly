#!/usr/bin/env python3
import json
import os
import sys
from datetime import datetime, timezone

import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, GLib


try:
    gi.require_version("AppIndicator3", "0.1")
    from gi.repository import AppIndicator3
    HAS_APPINDICATOR = True
except Exception:
    HAS_APPINDICATOR = False


APP_ID = "restly-controller"
QUEUE_DIR = os.path.expanduser("~/.config/restly/commands")
QUEUE_FILE = os.path.join(QUEUE_DIR, "queue.jsonl")


def ensure_dirs():
    os.makedirs(QUEUE_DIR, exist_ok=True)


def iso_now():
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat()


def append_command(action, params=None):
    ensure_dirs()
    payload = {
        "ts": iso_now(),
        "action": action,
    }
    if params:
        payload["params"] = params
    with open(QUEUE_FILE, "a", encoding="utf-8") as f:
        f.write(json.dumps(payload, ensure_ascii=False) + "\n")


class CommandDialog(Gtk.Dialog):
    def __init__(self, parent):
        super().__init__(title="Restly Command", transient_for=parent, flags=0)
        self.set_modal(True)
        self.set_default_size(420, 80)

        content = self.get_content_area()
        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=8, margin=12)
        content.add(box)

        self.entry = Gtk.Entry()
        self.entry.set_placeholder_text("e.g., Set a 45-minute deep work session")
        box.pack_start(self.entry, True, True, 0)

        self.add_button("Cancel", Gtk.ResponseType.CANCEL)
        self.add_button("Send", Gtk.ResponseType.OK)
        self.show_all()

    def get_text(self):
        return self.entry.get_text().strip()


class ControllerApp:
    def __init__(self):
        self.window = None
        if HAS_APPINDICATOR:
            self._init_indicator()
        else:
            self._init_window_fallback()

    def _init_indicator(self):

        icon_name = "preferences-system-time"
        self.indicator = AppIndicator3.Indicator.new(
            APP_ID, icon_name, AppIndicator3.IndicatorCategory.APPLICATION_STATUS
        )
        self.indicator.set_status(AppIndicator3.IndicatorStatus.ACTIVE)
        self.indicator.set_menu(self._build_menu())

    def _build_menu(self):
        menu = Gtk.Menu()

        item_quick_deep = Gtk.MenuItem(label="Start Deep Work (45m)")
        item_quick_deep.connect("activate", self.on_start_deep_45)
        menu.append(item_quick_deep)

        item_resched = Gtk.MenuItem(label="Reschedule Break…")
        item_resched.connect("activate", self.on_reschedule_break)
        menu.append(item_resched)

        item_pause = Gtk.MenuItem(label="Pause/Resume")
        item_pause.connect("activate", self.on_toggle_pause)
        menu.append(item_pause)

        item_summary = Gtk.MenuItem(label="Summarize Day")
        item_summary.connect("activate", self.on_summarize_day)
        menu.append(item_summary)

        menu.append(Gtk.SeparatorMenuItem())

        item_cmd = Gtk.MenuItem(label="Command…")
        item_cmd.connect("activate", self.on_command_palette)
        menu.append(item_cmd)

        menu.append(Gtk.SeparatorMenuItem())

        item_quit = Gtk.MenuItem(label="Quit Controller")
        item_quit.connect("activate", self.on_quit)
        menu.append(item_quit)

        menu.show_all()
        return menu

    def _init_window_fallback(self):
        self.window = Gtk.Window(title="Restly Controller")
        self.window.set_default_size(360, 120)
        self.window.connect("destroy", self.on_quit)

        grid = Gtk.Grid(column_spacing=8, row_spacing=8, margin=12)
        self.window.add(grid)

        btn_deep = Gtk.Button(label="Deep Work (45m)")
        btn_deep.connect("clicked", self.on_start_deep_45)
        grid.attach(btn_deep, 0, 0, 1, 1)

        btn_resched = Gtk.Button(label="Reschedule Break…")
        btn_resched.connect("clicked", self.on_reschedule_break)
        grid.attach(btn_resched, 1, 0, 1, 1)

        btn_pause = Gtk.Button(label="Pause/Resume")
        btn_pause.connect("clicked", self.on_toggle_pause)
        grid.attach(btn_pause, 0, 1, 1, 1)

        btn_summary = Gtk.Button(label="Summarize Day")
        btn_summary.connect("clicked", self.on_summarize_day)
        grid.attach(btn_summary, 1, 1, 1, 1)

        btn_cmd = Gtk.Button(label="Command…")
        btn_cmd.connect("clicked", self.on_command_palette)
        grid.attach(btn_cmd, 0, 2, 2, 1)

        self.window.show_all()


    def on_start_deep_45(self, *_args):
        append_command("set_session", {"duration_minutes": 45, "type": "deep_work"})

    def on_reschedule_break(self, *_args):
        parent = self.window if self.window else None
        dlg = CommandDialog(parent)
        dlg.entry.set_text("Reschedule my break for after this Zoom call")
        res = dlg.run()
        if res == Gtk.ResponseType.OK:
            text = dlg.get_text()
            if text:

                append_command("nl_command", {"text": text})
        dlg.destroy()

    def on_toggle_pause(self, *_args):
        append_command("toggle_pause")

    def on_summarize_day(self, *_args):
        append_command("summarize_day")

    def on_command_palette(self, *_args):
        parent = self.window if self.window else None
        dlg = CommandDialog(parent)
        res = dlg.run()
        if res == Gtk.ResponseType.OK:
            text = dlg.get_text()
            if text:
                append_command("nl_command", {"text": text})
        dlg.destroy()

    def on_quit(self, *_args):
        Gtk.main_quit()


def main():
    ensure_dirs()
    app = ControllerApp()
    Gtk.main()


if __name__ == "__main__":
    sys.exit(main())