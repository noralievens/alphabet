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



static gboolean on_click(Timeline* this, GdkEvent* event, GtkWidget* darea)
{
    if (!this->player->current) return FALSE;

    gint w = gtk_widget_get_allocated_width(darea);
    gdouble x;
    gdouble scale = this->player->current->length / w;

    gdk_event_get_axis(event, GDK_AXIS_X, &x);
    x *= scale;

    player_goto(this->player, x);
    return FALSE;
}

static gboolean on_draw(Timeline* this, cairo_t* cr, GtkWidget* darea)
{
    if (!this->player->current || !this->player->current->length) return FALSE;
    gint w = gtk_widget_get_allocated_width(darea);
    gint h = gtk_widget_get_allocated_height(darea);
    gdouble x;
    gdouble scale = this->player->current->length / w;

    /* draw loop */
    if (this->player->loop_start) {
        gdouble x_start = this->player->loop_start / scale;
        gdk_cairo_set_source_rgba(cr, &this->loop);
        cairo_set_line_width(cr, 2);
        cairo_move_to(cr, x_start, 0);
        cairo_line_to(cr, x_start, h);
        cairo_stroke(cr);
        if (this->player->loop_stop) {
            gdk_cairo_set_source_rgba(cr, &this->loop);
            gdouble x_end = this->player->loop_stop / scale;
            cairo_rectangle(cr, x_start, 0, x_end - x_start, h);
            cairo_fill(cr);
        }
    }

    /* draw marker */
    if (this->player->marker) {
        gdouble x_mark = this->player->marker / scale;
        gdk_cairo_set_source_rgba(cr, &this->marker);
        cairo_move_to(cr, x_mark, 0);
        cairo_line_to(cr, x_mark, h);
        cairo_stroke(cr);
    }

    /* draw position */
    x = this->player->position / scale;
    gdk_cairo_set_source_rgba(cr, &this->position);
    cairo_move_to(cr, x, 0);
    cairo_line_to(cr, x, h);
    cairo_stroke(cr);


    return FALSE;
}

void timeline_update(Timeline* this)
{
    gtk_widget_queue_draw(this->box);
}

Timeline* timeline_new(Player* player)
{
    Timeline* this = malloc(sizeof(Timeline));
    GtkWidget* darea;

    this->player = player;
    this->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    assert(gdk_rgba_parse(&this->position, "rgba(128,128,128,0.5)")
            && gdk_rgba_parse(&this->loop, "rgba(0,128,0,0.25)")
            && gdk_rgba_parse(&this->marker, "rgba(128,0,0,0.5)")
            && "allocate timeline colors");

    darea = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(this->box), darea, TRUE, TRUE, 0);

    gtk_widget_add_events(darea, GDK_BUTTON_PRESS_MASK);
    g_signal_connect_swapped(darea, "button-press-event", G_CALLBACK(on_click), this);
    g_signal_connect_swapped(darea, "draw", G_CALLBACK(on_draw), this);

    gtk_widget_show_all(this->box);

    return this;
}
