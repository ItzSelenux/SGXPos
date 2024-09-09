#include <gtk/gtk.h>
#include "sgxpos.h"

GtkWidget *window, *box, *label, *button;
int pos = 0;

void on_button_clicked(GtkWidget *widget, gpointer data)
{
	if (pos == 0)
	{
		sgxpos_set_edge(GTK_WINDOW(window), SGX_POS_EDGE_TOP, FALSE);
		sgxpos_set_edge(GTK_WINDOW(window), SGX_POS_EDGE_BOTTOM, TRUE);
		sgxpos_set_margin(GTK_WINDOW(window), SGX_POS_EDGE_BOTTOM, 16);
		pos = 1;
	}
	else
	{
		sgxpos_set_edge(GTK_WINDOW(window), SGX_POS_EDGE_BOTTOM, FALSE);
		sgxpos_set_edge(GTK_WINDOW(window), SGX_POS_EDGE_TOP, TRUE);
		sgxpos_set_margin(GTK_WINDOW(window), SGX_POS_EDGE_TOP, 16);
		pos = 0;
	}
}

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	// Window Details
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(window), "SGXPos Example");

		//prevent to don't use gtk_window_set_default_size since that adds extra margin
		//gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
		gtk_container_set_border_width(GTK_CONTAINER(window), 10);
		gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	//SGXPos Declarations:
	sgx_set_exclusive_zone(GTK_WINDOW(window));
	sgxpos_set_margin(GTK_WINDOW(window), SGX_POS_EDGE_BOTTOM, 16);
	sgxpos_set_edge(GTK_WINDOW(window), SGX_POS_EDGE_TOP, TRUE);

	// Window Content
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
		label = gtk_label_new("Welcome to GTK");
			gtk_label_set_markup(GTK_LABEL(label), "<span font='20'>Welcome to SGXPos</span>");
		button = gtk_button_new_with_label("Click = Hapiness");

		gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
		gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), box);

	//Signals
	g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), window);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}