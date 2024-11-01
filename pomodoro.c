#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *timer_label;
gint time_left; // time in seconds
gboolean timer_running = FALSE;
gboolean timer_paused = FALSE;
gboolean is_break = FALSE;

static void play_sound(const char *sound_file) {
    char command[100];
    snprintf(command, sizeof(command), "aplay %s", sound_file);
    system(command);
}

static void update_timer() {
    if (time_left > 0) {
        time_left--;
        int minutes = time_left / 60;
        int seconds = time_left % 60;
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        gtk_label_set_text(GTK_LABEL(timer_label), buffer);
    } else {
        if (is_break) {
            play_sound("break_end.wav");
            time_left = 25 * 60; // restart pomodoro

            gtk_label_set_text(GTK_LABEL(timer_label), "25:00");
        } else {
            timer_running = FALSE; // stop timer
        }
    }
}

static void start_timer(GtkWidget *widget, gpointer user_data) {
    if (timer_running && !timer_paused) return; // Don't start if it's running or is not paused
    if (timer_paused) {
        timer_paused = FALSE; // continue the timer
    } else {
        time_left = 25 * 60; // 25 minutes in seconds
        is_break = FALSE;
    }
    timer_running = TRUE;

    // Update timer each seconds
    g_timeout_add_seconds(1, (GSourceFunc) update_timer, NULL);
}

static void pause_timer(GtkWidget *widget, gpointer user_data) {
    if (timer_running) {
        timer_paused = TRUE;
        timer_paused = FALSE;
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *start_button;
    GtkWidget *pause_button;

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

    pause_button = gtk_button_new_with_label("Pause");
    g_signal_connect(pause_button, "clicked", G_CALLBACK(pause_timer), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), pause_button, TRUE, TRUE, 0);

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
