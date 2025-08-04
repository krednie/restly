#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/wait.h>
#include "popup.h"

void show_popup(const char *message, int gtk_dur) {
    // Fork a child process to handle the popup
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process - handles the popup
        int argc = 0;
        char **argv = NULL;
        gtk_init(&argc, &argv);

        GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        // gtk_window_set_title(GTK_WINDOW(window), "Restly Notification");
        gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
        gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
        gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);

        GtkWidget *label = gtk_label_new(message);
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add(GTK_CONTAINER(window), box);
        gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 10);

        GtkCssProvider *css = gtk_css_provider_new();
        gtk_css_provider_load_from_data(css,
            "window { background-color: #282c34; border-radius: 12px; }"
            "label { color: white; font-size: 16pt; font-weight: bold; }",
            -1, NULL);
        GtkStyleContext *context = gtk_widget_get_style_context(window);
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);

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

        // Auto-close after 5 seconds
        g_timeout_add_seconds(gtk_dur, (GSourceFunc)gtk_main_quit, NULL);

        gtk_main();
        exit(0); // Exit child process when popup closes
    } 
    else if (pid > 0) {
        // Parent process - clean up zombie processes
        int status;
        waitpid(pid, &status, WNOHANG); // Non-blocking wait
    }
    // If fork failed, just continue without popup
}
