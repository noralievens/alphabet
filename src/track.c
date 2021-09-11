/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : track.c
 */

#include "../include/track.h"


Track* track_new(GFile* file)
{
    Track* this = malloc(sizeof(Track));

    this->file = file;

    char* name = g_file_get_basename(file);
    this->name = calloc(strlen(name)+1, sizeof(char));
    strncpy((gchar*)this->name, name, strlen(name));

    char* uri = g_file_get_uri(file);
    this->uri = calloc(strlen(uri)+1, sizeof(char));
    strncpy((gchar*)this->uri, uri, strlen(uri));

    /* this->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); */

    /* this->label = gtk_label_new(this->name); */
    /* gtk_box_pack_start(GTK_BOX(this->box), this->label, FALSE, FALSE, 0); */

    /* TODO findout track length when loaded */
    this->length = 300;

    /* gtk_widget_show_all(this->box); */
    return this;
}

void track_free(Track* this)
{
    if (!this) return;

    /* if (this->box) gtk_widget_destroy(this->box); */
    if (this->name) free(this->name);
    /* cannot - must not unref file ??*/
    free(this);
}

void track_print(Track* this)
{
    printf("name = %s\n", this->name);
}
