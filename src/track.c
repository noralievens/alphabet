/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : track.c
 */

#include "../include/track.h"


Track* track_new(const char* name, const char* uri)
{
    Track* this = malloc(sizeof(Track));

    this->name = calloc(strlen(name)+1, sizeof(char));
    strncpy(this->name, name, strlen(name));

    this->uri = calloc(strlen(uri)+1, sizeof(char));
    strncpy(this->uri, uri, strlen(uri));

    /* TODO findout track length when loaded */
    this->length = 300;

    this->offset = 0;
    this->gain = 0;

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
