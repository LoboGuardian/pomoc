#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
GtkWidget *timer_label;
gint time_left, work_time, break_time;
gboolean timer_running = FALSE, timer_paused = FALSE, is_break = FALSE;
GSource *timer_source = NULL;

// Function to play a sound
static void play_sound(const char *sound_file) {
    char command[100];
    snprintf(command, sizeof(command), "aplay %s", sound_file);
    system(command);
}

// Function to update the timer label
static gboolean update_timer(gpointer user_data) {
    if (time_left > 0) {
        time_left--;
        int minutes = time_left / 60;
        int seconds = time_left % 60;
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        gtk_label_set_text(GTK_LABEL(timer_label), buffer);

        // Update progress bar
        double progress = (double)(work_time - time_left) / work_time;
        // gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), progress);
    } else {
        if (is_break) {
            play_sound("break_end.wav");
            time_left = work_time;
            is_break = FALSE;
            gtk_label_set_text(GTK_LABEL(timer_label), "Start Working");
        } else {
            play_sound("work_end.wav");
            time_left = break_time;
            is_break = TRUE;
            gtk_label_set_text(GTK_LABEL(timer_label), "Take a Break");
            // timer_running = FALSE; // stop timer
        }
    }
    return timer_running; // Keep the timer running until it stopped
}

// Function to start the timer
static void start_timer(GtkWidget *widget, gpointer user_data) {
    if (timer_running && !timer_paused) return; // Don't start if it's running or is not paused
    if (timer_paused) {
        timer_paused = FALSE; // continue the timer
    } else {
        time_left = 25 * 60; // 25 minutes in seconds
        break_time = 5 * 60; // 5 minutes break
        time_left = work_time;
        is_break = FALSE;
        gtk_label_set_text(GTK_LABEL(timer_label), "25:00");
    }
    timer_running = TRUE;

    // Update timer each seconds
    g_timeout_add_seconds(1, (GSourceFunc) update_timer, NULL);
}

// Function to pause the timer
static void pause_timer(GtkWidget *widget, gpointer user_data) {
    if (timer_running) {
        timer_paused = TRUE;
        // g_source_remove(timer_source);
        timer_source = NULL;
    }
}

// Functoon to stop the timer
static void stop_timer(GtkWidget *widget, gpointer user_data) {
    timer_running = FALSE;
    timer_paused = FALSE;
    // g_source_remove(timer_source);
    timer_source = NULL;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *start_button;
    GtkWidget *pause_button;
    GtkWidget *stop_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Pomodoro");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 100);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    timer_label = gtk_label_new("25:00");
    // gtk_widget_modify_font(timer_label, pango_font_description_from_string("Bold 50")); // Aumentar el tamaño del texto
    gtk_box_pack_start(GTK_BOX(vbox), timer_label, TRUE, TRUE, 0);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5); // Crear un contenedor horizontal para los botones
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
    
    start_button = gtk_button_new_with_label("Start");
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_timer), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), start_button, TRUE, TRUE, 0);

    pause_button = gtk_button_new_with_label("Pause");
    g_signal_connect(pause_button, "clicked", G_CALLBACK(pause_timer), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), pause_button, TRUE, TRUE, 0);

    stop_button = gtk_button_new_with_label("Stop");
    g_signal_connect(stop_button, "clicked", G_CALLBACK(stop_timer), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), stop_button, TRUE, TRUE, 0);

    // progress_bar = gtk_progress_bar_new();
    // gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0,0);
    // gtk_box_pack_start(GTK_BOX(vbox), progress_bar, TRUE, TRUE, 0);

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
