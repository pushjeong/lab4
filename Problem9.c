#include <gtk/gtk.h>
#include <stdlib.h>

// Callback function for the buttons
void on_operation_clicked(GtkWidget *widget, gpointer data) {
    const char *operation = (const char *)data;

    // Get the input fields and result label
    GtkWidget *entry1 = g_object_get_data(G_OBJECT(widget), "entry1");
    GtkWidget *entry2 = g_object_get_data(G_OBJECT(widget), "entry2");
    GtkWidget *result_label = g_object_get_data(G_OBJECT(widget), "result_label");

    const char *text1 = gtk_entry_get_text(GTK_ENTRY(entry1));
    const char *text2 = gtk_entry_get_text(GTK_ENTRY(entry2));

    // Convert input to numbers
    double num1 = atof(text1);
    double num2 = atof(text2);
    double result = 0.0;
    char result_text[100] = {0};

    // Perform the operation
    if (g_strcmp0(operation, "add") == 0) {
        result = num1 + num2;
        snprintf(result_text, sizeof(result_text), "Result: %.2f", result);
    } else if (g_strcmp0(operation, "subtract") == 0) {
        result = num1 - num2;
        snprintf(result_text, sizeof(result_text), "Result: %.2f", result);
    } else if (g_strcmp0(operation, "multiply") == 0) {
        result = num1 * num2;
        snprintf(result_text, sizeof(result_text), "Result: %.2f", result);
    } else if (g_strcmp0(operation, "divide") == 0) {
        if (num2 != 0) {
            result = num1 / num2;
            snprintf(result_text, sizeof(result_text), "Result: %.2f", result);
        } else {
            snprintf(result_text, sizeof(result_text), "Error: Division by zero");
        }
    }

    // Update the result label
    gtk_label_set_text(GTK_LABEL(result_label), result_text);
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *grid, *entry1, *entry2, *add_button, *subtract_button, *multiply_button, *divide_button, *result_label;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a grid layout
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create input fields
    entry1 = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry1, 0, 0, 2, 1);
    entry2 = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry2, 0, 1, 2, 1);

    // Create result label
    result_label = gtk_label_new("Result: ");
    gtk_grid_attach(GTK_GRID(grid), result_label, 0, 2, 2, 1);

    // Create buttons
    add_button = gtk_button_new_with_label("Add");
    subtract_button = gtk_button_new_with_label("Subtract");
    multiply_button = gtk_button_new_with_label("Multiply");
    divide_button = gtk_button_new_with_label("Divide");

    // Attach buttons to the grid
    gtk_grid_attach(GTK_GRID(grid), add_button, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), subtract_button, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), multiply_button, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), divide_button, 1, 4, 1, 1);

    // Set button callbacks
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_operation_clicked), "add");
    g_signal_connect(subtract_button, "clicked", G_CALLBACK(on_operation_clicked), "subtract");
    g_signal_connect(multiply_button, "clicked", G_CALLBACK(on_operation_clicked), "multiply");
    g_signal_connect(divide_button, "clicked", G_CALLBACK(on_operation_clicked), "divide");

    // Store entry and result label in button's data
    g_object_set_data(G_OBJECT(add_button), "entry1", entry1);
    g_object_set_data(G_OBJECT(add_button), "entry2", entry2);
    g_object_set_data(G_OBJECT(add_button), "result_label", result_label);

    g_object_set_data(G_OBJECT(subtract_button), "entry1", entry1);
    g_object_set_data(G_OBJECT(subtract_button), "entry2", entry2);
    g_object_set_data(G_OBJECT(subtract_button), "result_label", result_label);

    g_object_set_data(G_OBJECT(multiply_button), "entry1", entry1);
    g_object_set_data(G_OBJECT(multiply_button), "entry2", entry2);
    g_object_set_data(G_OBJECT(multiply_button), "result_label", result_label);

    g_object_set_data(G_OBJECT(divide_button), "entry1", entry1);
    g_object_set_data(G_OBJECT(divide_button), "entry2", entry2);
    g_object_set_data(G_OBJECT(divide_button), "result_label", result_label);

    // Show all widgets
    gtk_widget_show_all(window);

    // Run GTK main loop
    gtk_main();

    return 0;
}
