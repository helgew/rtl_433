/*
 * TODO: Licence header
 * Author: TODO: your name
 */

#include <stdint.h> // for uint8_t
#include <stdio.h>  // fprintf
#include <string.h> // memcmp
#include <stdlib.h> // abs

#include <time.h>

#include "plugin.h"         // for generic plugin_descriptor
#include "rtl_433_plugin.h" // For application plugin descriptor

/**
 *  Decoding for Steffen Switch Transmitter
 *  TODO: Add references (to hopefully permanent URL's) where more detailed info can be found
 */

/* from plugin.h: */

//
// typedef struct plugin_descriptor_t {
//     char *application;  //! For what application was this plugin written
//     char *type;         //! What type of plugin (application specific)
//     char *model;        //! The specific model for which the plugin is written
//     int version;        //! The version of the plugin
// } plugin_descriptor;

/* Function declarations */

static int steffen_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

r_device steffen = {
    /* .id             = */ 9,
    /* .name           = */ "Steffen Switch Transmitter",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 140,
    /* .long_limit     = */ 270,
    /* .reset_limit    = */ 1500,
    /* .json_callback  = */ &steffen_callback,
};

rtl_433_plugin_t plugin =
{
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "remote.button",
        .model       = "Steffen",
        .version     = 1
    },
    .r_device_p = &steffen
};

VISIBLE extern void *get_plugin()
{
    static int returned = 0;
    if ( returned == 0)
    {
        returned = 1;
        return &plugin;
    }
    return NULL;

}

static int steffen_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {

// TODO remove or replace by debug output call
    fprintf(stderr, "Called callback plugin for App: %s, type: %s, model: %s, version: %d\n",
        plugin.plugin_desc.application,
        plugin.plugin_desc.type,
        plugin.plugin_desc.model,
        plugin.plugin_desc.version );

    if (bb[0][0]==0x00 && ((bb[1][0]&0x07)==0x07) && bb[1][0]==bb[2][0] && bb[2][0]==bb[3][0]) {

        fprintf(stderr, "Remote button event:\n");
        fprintf(stderr, "model   = Steffan Switch Transmitter\n");
        fprintf(stderr, "code    = %d%d%d%d%d\n", (bb[1][0]&0x80)>>7, (bb[1][0]&0x40)>>6, (bb[1][0]&0x20)>>5, (bb[1][0]&0x10)>>4, (bb[1][0]&0x08)>>3);

        if ((bb[1][2]&0x0f)==0x0e)
            fprintf(stderr, "button  = A\n");
        else if ((bb[1][2]&0x0f)==0x0d)
            fprintf(stderr, "button  = B\n");
        else if ((bb[1][2]&0x0f)==0x0b)
            fprintf(stderr, "button  = C\n");
        else if ((bb[1][2]&0x0f)==0x07)
            fprintf(stderr, "button  = D\n");
        else if ((bb[1][2]&0x0f)==0x0f)
            fprintf(stderr, "button  = ALL\n");
        else
            fprintf(stderr, "button  = unknown\n");

        if ((bb[1][2]&0xf0)==0xf0) {
            fprintf(stderr, "state   = OFF\n");
        } else {
            fprintf(stderr, "state   = ON\n");
        }

        return 1;
    }
    return 0;
}
