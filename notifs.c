#include <libnotify/notify.h>
#include <glib.h>
#include "notifs.h"

void send_notif(const char *title, const char *message)
{
    notify_init("restly");

    NotifyNotification *notification  = notify_notification_new(title, message, NULL);

    notify_notification_set_timeout(notification, 10000);
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);

    notify_notification_show(notification, NULL);


    g_object_unref(G_OBJECT(notification)); 
    notify_uninit();

    return;
}