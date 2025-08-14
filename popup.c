#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include "popup.h"

static pid_t current_popup_pid = 0;

static gboolean on_window_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    return FALSE; // allow normal drawing (CSS backgrounds on children)
}

void show_popup(const char *message, int gtk_dur) {
    // Kill previous popup if it exists
    if (current_popup_pid > 0) {
        kill(current_popup_pid, SIGTERM);
        waitpid(current_popup_pid, NULL, 0);
        current_popup_pid = 0;
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {
        int argc = 0;
        char **argv = NULL;
        unsetenv("DESKTOP_STARTUP_ID");
        gtk_init(&argc, &argv);

        GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        // gtk_window_set_title(GTK_WINDOW(window), "Restly Notification");
        gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
        gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
        gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
        gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_NOTIFICATION);
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
        gtk_window_set_skip_pager_hint(GTK_WINDOW(window), TRUE);
        gtk_window_set_accept_focus(GTK_WINDOW(window), FALSE);
        gtk_window_set_focus_on_map(GTK_WINDOW(window), FALSE);
        gtk_widget_set_app_paintable(window, TRUE);
        g_signal_connect(window, "draw", G_CALLBACK(on_window_draw), NULL);

        GdkScreen *screen = gdk_screen_get_default();
        GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
        if (visual != NULL) {
            gtk_widget_set_visual(window, visual);
        }

        GtkWidget *label = gtk_label_new(message);
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_name(box, "toast");
        gtk_container_add(GTK_CONTAINER(window), box);
        gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 10);

        GtkCssProvider *css = gtk_css_provider_new();
        gtk_css_provider_load_from_data(css,
            "#toast { background-color: rgba(245, 250, 255, 0.95); border: 5px solid #a0c4ff; border-radius: 12px; padding: 12px; }"
            "label { color: black; font-size: 16pt; font-weight: bold; }",
            -1, NULL);
        gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);

        GdkDisplay *display = gdk_display_get_default();
        GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
        GdkRectangle geometry;
        gdk_monitor_get_geometry(monitor, &geometry);

        int screen_width = geometry.width;
        int screen_height = geometry.height;

        gint x = screen_width - 320;
        gint y = screen_height - 120;
        gtk_window_move(GTK_WINDOW(window), x, y);

        gtk_widget_show_all(window);
        g_timeout_add_seconds(gtk_dur, (GSourceFunc)gtk_main_quit, NULL);

        gtk_main();
        exit(0);
    } 
    else if (pid > 0) {
        current_popup_pid = pid;
    }
}