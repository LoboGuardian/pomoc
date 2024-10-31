#include <gtk/gtk.h>

GtkWidget *timer_label;
gint time_left; // time in seconds
gboolean timer_running = FALSE;

static void update_timer() {
    if (time_left > 0) {
        time_left--;
        int minutes = time_left / 60;
        int seconds = time_left % 60;
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        gtk_label_set_text(GTK_LABEL(timer_label), buffer);
    } else {
        timer_running = FALSE; // stop timer
    }
}

static void start_timer(GtkWidget *widget, gpointer user_data) {
    if (timer_running) return; // Don't start if it's running
    time_left = 25 * 60; // 25 minutes in seconds
    timer_running = TRUE;

    // Update timer each seconds
    g_timeout_add_seconds(1, (GSourceFunc) update_timer, NULL);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *start_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Pomodoro");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 100);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    timer_label = gtk_label_new("25:00");
    gtk_box_pack_start(GTK_BOX(vbox), timer_label, TRUE, TRUE, 0);

    start_button = gtk_button_new_with_label("Start");
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_timer), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), start_button, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.jarwarez.pomodorotimer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
