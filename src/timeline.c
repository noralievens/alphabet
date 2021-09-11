/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 11/09/2021
 * @filename    : timeline.c
 */


#include <assert.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "../include/track.h"
#include "../include/player.h"

#include "../include/timeline.h"


GdkRGBA current, loop, marker;

static gboolean on_click(GtkWidget* this, GdkEvent* event, Player* player)
{
    gint w = gtk_widget_get_allocated_width(this);
    gdouble x;
    gdk_event_get_axis(event, GDK_AXIS_X, &x);
    x *= (player->current->length / w);

    player_goto(player, x);
    return TRUE;
}


static gboolean on_draw(GtkWidget* this, cairo_t* cr, Player* player)
{
    gint w = gtk_widget_get_allocated_width(this);
    gint h = gtk_widget_get_allocated_height(this);
    gdouble x;
    gdouble scale = player->current->length / w;

    /* draw loop */
    if (player->loop_start) {
        gdouble x_start = player->loop_start / scale;
        gdk_cairo_set_source_rgba(cr, &loop);
        cairo_set_line_width(cr, 2);
        cairo_move_to(cr, x_start, 0);
        cairo_line_to(cr, x_start, h);
        cairo_stroke(cr);
        if (player->loop_stop) {
            gdk_cairo_set_source_rgba(cr, &loop);
            gdouble x_end = player->loop_stop / scale;
            cairo_rectangle(cr, x_start, 0, x_end - x_start, h);
            cairo_fill(cr);
        }
    }

    /* draw marker */
    if (player->marker) {
        gdouble x_mark = player->marker / scale;
        gdk_cairo_set_source_rgba(cr, &marker);
        cairo_move_to(cr, x_mark, 0);
        cairo_line_to(cr, x_mark, h);
        cairo_stroke(cr);
    }

    /* draw position */
    x = player->position / scale;
    cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1);
    cairo_move_to(cr, x, 0);
    cairo_line_to(cr, x, h);
    cairo_stroke(cr);

    printf("pos: %f\n", player->position);

    return FALSE;
}

GtkWidget* timeline_new(Player* player)
{
    GtkWidget* darea;

    assert(gdk_rgba_parse(&current, "rgba(127,127,127,0.5)")
            && gdk_rgba_parse(&loop, "rgba(0,127,0,0.25)")
            && gdk_rgba_parse(&marker, "rgba(127,0,0,0.5)")
            && "allocate timeline colors");

    darea = gtk_drawing_area_new();
    gtk_widget_add_events(darea, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(darea, "button-press-event", G_CALLBACK(on_click), player);
    g_signal_connect(darea, "draw", G_CALLBACK(on_draw), player);

    return darea;
}

