#ifndef MILIB_H
#define MILIB_H

#include <X11/Xlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>

// Define equivalents to gtk_layer_set_layer
#define gtk_layer_set_anchor sgxpos_set_edge 
#define gtk_layer_set_margin sgxpos_set_margin
#define gtk_layer_set_layer sgx_set_layer
#define gtk_layer_auto_exclusive_zone_enable sgx_set_exclusive_zone
#define gtk_layer_init_for_window sgx_set_exclusive_zone //this is not exact equivalent but solve exclusive zone problems

#define GTK_LAYER_SHELL_EDGE_TOP SGX_POS_EDGE_TOP
#define GTK_LAYER_SHELL_EDGE_BOTTOM SGX_POS_EDGE_BOTTOM
#define GTK_LAYER_SHELL_EDGE_LEFT SGX_POS_EDGE_LEFT
#define GTK_LAYER_SHELL_EDGE_RIGHT SGX_POS_EDGE_RIGHT
#define GTK_LAYER_SHELL_EDGE_RIGHT SGX_POS_EDGE_RIGHT
#define GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER SGX_POS_EDGE_COUNT

#define GTK_LAYER_SHELL_LAYER_OVERLAY SGX_LAYER_DOCK
#define GTK_LAYER_SHELL_LAYER_TOP SGX_LAYER_DOCK
#define GTK_LAYER_SHELL_LAYER_BOTTOM SGX_LAYER_BACKGROUND
#define GTK_LAYER_SHELL_LAYER_BACKGROUND SGX_LAYER_BACKGROUND

//Define unsupported functions (for the future? Doesn't actually do anything)
void SGPass()
{
	g_info("Not Implemented");
}

typedef enum
{
	SGX_POS_EDGE_LEFT,
	SGX_POS_EDGE_RIGHT,
	SGX_POS_EDGE_TOP,
	SGX_POS_EDGE_BOTTOM,
	SGX_POS_EDGE_NONE,
	//SGX_POS_EDGE_CENTER,
	//SGX_POS_EDGE_TOPLEFT,
	//SGX_POS_EDGE_TOPRIGHT,
	//SGX_POS_EDGE_BOTTOMRIGHT,
	//SGX_POS_EDGE_BOTTOMLEFT,
	SGX_POS_EDGE_COUNT,
} sgxposedge;

typedef enum
{
	SGX_LAYER_MENU,
	SGX_LAYER_DOCK,
	SGX_LAYER_NORMAL,
	SGX_LAYER_BELOW,
	SGX_LAYER_BACKGROUND,
	SGX_LAYER_COUNT,
} sgxlayer;

Display *SGXDisplay;

static sgxposedge TMPEDGE;
static sgxlayer TMPLAYER;
static gboolean TMPREDIM, SGXSETEXCLUSIVEZONE=0, WTOP, WBOTTOM, WLEFT, WRIGHT;

static int edgewrapped = 0, layerwrapped = 0, strutwrapped=0;

static gboolean top_set = FALSE, bottom_set = FALSE, left_set = FALSE, right_set = FALSE;

static int original_width = -1, original_height = -1, mtop = 0, mbottom = 0, mleft = 0, mright = 0;

int sgxpos_set_edge(GtkWindow *window, sgxposedge edge, gboolean redim);
int sgx_set_layer(GtkWindow *window, sgxlayer layer);
int sgx_set_exclusive_zone(GtkWindow *window);

int SGXPos_VERSION=0.1;

int SGXOpenDisplay()
{
	if (SGXDisplay == NULL)
	{
		SGXDisplay = XOpenDisplay(NULL);
		if (!SGXDisplay)
		{
			g_warning("Can't open X Display\n");
			return 0;
		}
	}
	return 1;
}

int sgxpos_get_screen_size(int *width, int *height)
{
	SGXOpenDisplay();

	if (SGXDisplay == NULL)
	{
		g_warning("Can't open display");
		return -1;
	}

	Screen *screen = XDefaultScreenOfDisplay(SGXDisplay);
	if (screen)
	{
		*width = XWidthOfScreen(screen);
		*height = XHeightOfScreen(screen);
		return 1;
	}
	else
	{
		g_warning("Screen is not valid");
		return -1;
	}
	return 0;
}

int SGXSetStrutPartial(GtkWindow *window)
{

	GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
	if (gdk_window == NULL)
	{
		g_warning("Window is not available");
		return 0;
	}

	Window xid = GDK_WINDOW_XID(gdk_window);

	GdkDisplay *display = gdk_window_get_display(gdk_window);
	Display *xdisplay = GDK_DISPLAY_XDISPLAY(display);

	Atom wm_strut_partial = XInternAtom(xdisplay, "_NET_WM_STRUT_PARTIAL", False);
	Atom wm_supporting_wm_check = XInternAtom(xdisplay, "_NET_SUPPORTING_WM_CHECK", False);

	if (wm_strut_partial == None || wm_supporting_wm_check == None)
	{
		g_warning("Failed to get Atoms");
		return 0;
	}

	GtkAllocation allocation;
	gtk_widget_get_allocation(GTK_WIDGET(window), &allocation);

	unsigned long ptop = 0;
	unsigned long pbottom = 0;
	unsigned long pleft = 0;
	unsigned long pright = 0;

	unsigned long left_start_y = 0, left_end_y = 0;
	unsigned long right_start_y = 0, right_end_y = 0;
	unsigned long top_start_x = 0, top_end_x = 0;
	unsigned long bottom_start_x = 0, bottom_end_x = 0;

	if (WTOP)
	{
		ptop = allocation.height+mbottom;
		top_start_x = allocation.x;
		top_end_x = allocation.x + allocation.width - 1;
	}
	else if (WBOTTOM)
	{
		pbottom = allocation.height+mtop;
		bottom_start_x = allocation.x;
		bottom_end_x = allocation.x + allocation.width - 1;
	}
	else if (WLEFT)
	{
		pleft = allocation.width+mright;
		left_start_y = allocation.y;
		left_end_y = allocation.y + allocation.height - 1;
	}
	else if (WRIGHT)
	{
		pright = allocation.width+mleft;
		right_start_y = allocation.y;
		right_end_y = allocation.y + allocation.height - 1;
	}

	// [left, right, top, bottom, left_start_y, left_end_y, right_start_y, right_end_y, top_start_x, top_end_x, bottom_start_x, bottom_end_x]
	unsigned long strut[12] = {pleft, pright, ptop, pbottom, 
		left_start_y, left_end_y, 
		right_start_y, right_end_y, 
		top_start_x, top_end_x, 
		bottom_start_x, bottom_end_x};

	XChangeProperty(xdisplay, xid, wm_strut_partial, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)strut, 12);

	XFlush(xdisplay);
return 1;
}

int SGXClearStrutPartial(GtkWindow *window)
{
  GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
    if (gdk_window == NULL)
    {
        g_warning("Window is not available");
        return 0;
    }

    Window xid = GDK_WINDOW_XID(gdk_window);

    GdkDisplay *display = gdk_window_get_display(gdk_window);
    Display *xdisplay = GDK_DISPLAY_XDISPLAY(display);

    Atom wm_strut_partial = XInternAtom(xdisplay, "_NET_WM_STRUT_PARTIAL", False);

    if (wm_strut_partial == None)
    {
        g_warning("Failed to get Atom _NET_WM_STRUT_PARTIAL");
        return 0;
    }

    XDeleteProperty(xdisplay, xid, wm_strut_partial);

    XFlush(xdisplay);

    return 1;
}

void wrapper_sgxpos_set_edge(GtkWindow *tmpwindow)
{
	sgxpos_set_edge(tmpwindow, TMPEDGE, TMPREDIM);
}

int sgxpos_set_edge(GtkWindow *window, sgxposedge edge, gboolean redim)
{
	if (!redim)
	{
		switch (edge)
		{
			case SGX_POS_EDGE_TOP:
				top_set = FALSE;
				mbottom = 0;
				WTOP = FALSE;
				break;
			case SGX_POS_EDGE_BOTTOM:
				bottom_set = FALSE;
				mtop = 0;
				WBOTTOM = FALSE;
				break;
			case SGX_POS_EDGE_LEFT:
				left_set = FALSE;
				mright = 0;
				WLEFT = FALSE;
				break;
			case SGX_POS_EDGE_RIGHT:
				right_set = FALSE;
				mleft = 0;
				WRIGHT = FALSE;
				break;
			case SGX_POS_EDGE_NONE:
				break;
			default:
				g_info("Not Implemented");
		}
		SGXClearStrutPartial(window);
		return 0;
	}

	if (!gtk_widget_get_visible(GTK_WIDGET(window)) && edgewrapped == 0)
	{
		TMPEDGE = edge;
		TMPREDIM = redim;
		g_signal_connect(window, "realize", G_CALLBACK(wrapper_sgxpos_set_edge), window);
		edgewrapped = 1;
		return 0;
	}

	gtk_window_set_default_size(GTK_WINDOW(window), 1, 1);
	gint swidth, sheight;

	if (sgxpos_get_screen_size(&swidth, &sheight))
	{
		int wwidth, wheight;
		gtk_window_get_size(GTK_WINDOW(window), &wwidth, &wheight);

		if (original_width == -1 && original_height == -1)
		{
			original_width = wwidth;
			original_height = wheight;
		}

		switch (edge)
		{
			case SGX_POS_EDGE_TOP:
				top_set = TRUE;
				break;
			case SGX_POS_EDGE_BOTTOM:
				bottom_set = TRUE;
				break;
			case SGX_POS_EDGE_LEFT:
				left_set = TRUE;
				break;
			case SGX_POS_EDGE_RIGHT:
				right_set = TRUE;
				break;
			case SGX_POS_EDGE_NONE:
				//gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
				break;
			default:
				g_info("Not Implemented");
		}

		int new_x = 0, new_y = 0, new_width = original_width, new_height = original_height;

		if (left_set && right_set)
		{
			new_x = 0;
			new_width = swidth;
		}
		else if (left_set)
		{
			new_x = 0;
		}
		else if (right_set)
		{
			new_x = swidth - original_width;
		}
		else
		{
			new_x = (swidth - original_width) / 2;
		}

		if (top_set && bottom_set)
		{
			new_y = 0;
			new_height = sheight;
		}
		else if (top_set)
		{
			new_y = 0;
		}
		else if (bottom_set)
		{
			new_y = sheight - original_height;
		}
		else
		{
			new_y = (sheight - original_height) / 2;
		}

		if (left_set && right_set)
		{
			new_height = original_height;
		}

		if (top_set && bottom_set)
		{
			new_width = original_width;
		}

		if ((top_set && left_set && right_set) || (top_set && !bottom_set && !left_set && !right_set))
		{
			WTOP = 1;
		}
		else if ((bottom_set && left_set && right_set) || (!top_set && bottom_set && !left_set && !right_set))
		{
			WBOTTOM = 1;
		}
		else if ((top_set && bottom_set && left_set) || (!top_set && !bottom_set && left_set && !right_set))
		{
			WLEFT = 1;
		}
		else if ((top_set && bottom_set && right_set) || (!top_set && !bottom_set && !left_set && right_set))
		{
			WRIGHT = 1;
		}

		gtk_window_move(GTK_WINDOW(window), new_x, new_y);
			gtk_window_resize(GTK_WINDOW(window), new_width, new_height);

		if (SGXSETEXCLUSIVEZONE)
		{
			if (gtk_widget_get_visible(GTK_WIDGET(window)))
			{
				SGXSetStrutPartial(window);
				strutwrapped = 1;
			}
		}
	}
	else
	{
		g_warning("Can't get screen size");
	}
	return 0;
}

void SGXSetLayer(GtkWindow *window, char *windowtype)
{
	GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
	if (gdk_window == NULL && layerwrapped == 0)
	{
		g_warning("Window is not available");
		return;
	}

	Window xid = GDK_WINDOW_XID(gdk_window);

	GdkDisplay *display = gdk_window_get_display(gdk_window);
	Display *xdisplay = GDK_DISPLAY_XDISPLAY(display);
	
	Atom wm_window_type = XInternAtom(xdisplay, "_NET_WM_WINDOW_TYPE", False);
	Atom wm_window_type_dock = XInternAtom(xdisplay, windowtype, False);

	if (wm_window_type == None || wm_window_type_dock == None)
	{
		g_warning("Failed to get Atoms");
		return;
	}

	XChangeProperty(xdisplay, xid, wm_window_type, XA_ATOM, 32,
		PropModeReplace, (unsigned char *)&wm_window_type_dock, 1);

	XFlush(xdisplay);
}

void SGXSetWindowState(GtkWindow *window, char *state)
{
	GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
	if (gdk_window == NULL)
	{
		g_warning("Window is not available");
		return;
	}

	Window xid = GDK_WINDOW_XID(gdk_window);

	GdkDisplay *display = gdk_window_get_display(gdk_window);
	Display *xdisplay = GDK_DISPLAY_XDISPLAY(display);
	
	Atom wm_state = XInternAtom(xdisplay, "_NET_WM_STATE", False);
	Atom wm_state_atom = XInternAtom(xdisplay, state, False);

	if (wm_state == None || wm_state_atom == None)
	{
		g_warning("Failed to get Atoms");
		return;
	}

	XChangeProperty(xdisplay, xid, wm_state, XA_ATOM, 32,
		PropModeReplace, (unsigned char *)&wm_state_atom, 1);

	XFlush(xdisplay);
}

int sgxpos_set_padding(GtkWindow *window, sgxposedge edge, gint N)
{
	gint wx, wy;
	gtk_window_get_position(GTK_WINDOW(window), &wx, &wy);

	if (edge == SGX_POS_EDGE_TOP)
	{
		gtk_window_move(GTK_WINDOW(window), wx, wy + N);
	}
	else if (edge == SGX_POS_EDGE_BOTTOM)
	{
		gtk_window_move(GTK_WINDOW(window), wx, wy - N);
	}
	else if (edge == SGX_POS_EDGE_LEFT)
	{
		gtk_window_move(GTK_WINDOW(window), wx + N, wy);
	}
	else if (edge == SGX_POS_EDGE_RIGHT)
	{
		gtk_window_move(GTK_WINDOW(window), wx - N, wy);
	}
	else if (edge == SGX_POS_EDGE_COUNT)
	{
		
	}
	else
	{
		g_warning("Argument not valid, assuming SGX_POS_EDGE_LEFT");
		gtk_window_move(GTK_WINDOW(window), wx + N, wy);
	}
return 1;
}

int sgxpos_set_margin(GtkWindow *window, sgxposedge edge, int N)
{
	switch (edge)
	{
		case SGX_POS_EDGE_TOP:
			mtop = N;
			break;
		case SGX_POS_EDGE_BOTTOM:
			mbottom = N;
			break;
		case SGX_POS_EDGE_LEFT:
			mleft = N;
			break;
		case SGX_POS_EDGE_RIGHT:
			mright = N;
			break;
		default:
			break;
	}
	return 0;
}

void wrapper_sgxpos_set_layer(GtkWindow *tmpwindow)
{
	sgx_set_layer(tmpwindow, TMPLAYER);
}

int sgx_set_layer(GtkWindow *window, sgxlayer layer)
{
	if (!gtk_widget_get_visible(GTK_WIDGET(window)) && layerwrapped == 0)
	{
		TMPLAYER = layer;
		g_signal_connect(window, "realize", G_CALLBACK(wrapper_sgxpos_set_layer), window);
		layerwrapped = 1;
		return 0;
	}

	if (layer == SGX_LAYER_MENU)
		{
			SGXSetLayer(window, "_NET_WM_WINDOW_TYPE_MENU");
		}
		else if (layer == SGX_LAYER_DOCK)
		{
			SGXSetLayer(window, "_NET_WM_WINDOW_TYPE_DOCK");
		}
		else if (layer == SGX_LAYER_NORMAL)
		{
			SGXSetLayer(window, "_NET_WM_WINDOW_TYPE_NORMAL");
		}
		else if (layer == SGX_LAYER_BELOW)
		{
			SGXSetWindowState(window, "_NET_WM_STATE_BELOW");
		}
		else if (layer == SGX_LAYER_BACKGROUND)
		{
			SGXSetLayer(window, "_NET_WM_WINDOW_TYPE_DESKTOP");
		}
	else
	{
		SGXSetLayer(window, "_NET_WM_WINDOW_TYPE_NORMAL");
	}
return 1;
}

void wrapper_sgx_sez(GtkWindow *window)
{
	sgxpos_set_edge(window, SGX_POS_EDGE_NONE, TRUE);
}

int sgx_set_exclusive_zone(GtkWindow *window)
{
	SGXSETEXCLUSIVEZONE=1;
	//if (!gtk_widget_get_visible(GTK_WIDGET(window)) && ezwrapped == 0) 
	//{
		//g_signal_connect(window, "realize", G_CALLBACK(wrapper_sgx_sez), window);
		//ezwrapped = 1;
		//return 0;
	//}
	//else
	//{
		//sgxpos_set_edge(window, SGX_POS_EDGE_NONE, TRUE);
	//}
return 1;
}
#endif