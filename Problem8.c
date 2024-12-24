#include <gtk/gtk.h>

// Button click event callback function
void on_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Button clicked!\n");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;   // Window
    GtkWidget *button;   // Button

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK+ GUI Program");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Connect window close event
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create button
    button = gtk_button_new_with_label("Click Me");

    // Connect button click event
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    // Add button to the window
    gtk_container_add(GTK_CONTAINER(window), button);

    // Display window
    gtk_widget_show_all(window);

    // Run GTK main loop
    gtk_main();

    return 0;
}
