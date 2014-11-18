#include <stdint.h> // for uint8_t
#include <stdio.h>  // fprintf
#include <string.h> // memcmp
#include <stdlib.h> // abs

#include <time.h>

#include "plugin.h"         // for generic plugin_descriptor
#include "rtl_433_plugin.h" // For application plugin descriptor

/**
 *  Provide information about the protocol and compatible transmitter devices here
 *  Add references (to hopefully permanent URL's) where more detailed info can be found
 *  Note that this info should be picked up by Doxygen to provide documentation for this module
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

static int waveman_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

r_device waveman = {
    /* .id             = */ 6,
    /* .name           = */ "Waveman Switch Transmitter",
    /* .modulation     = */ OOK_PWM_P,
    /* .short_limit    = */ 1000/4,
    /* .long_limit     = */ 8000/4,
    /* .reset_limit    = */ 30000/4,
    /* .json_callback  = */ &waveman_callback,
};


rtl_433_plugin_t plugin =
{
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "button.*",
        .model       = "any",
        .version     = 1
    },
    .r_device_p = &waveman
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

static int waveman_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {

// TODO remove or replace by debug output call
    fprintf(stderr, "Called callback plugin for App: %s, type: %s, model: %s, version: %d\n",
        plugin.plugin_desc.application,
        plugin.plugin_desc.type,
        plugin.plugin_desc.model,
        plugin.plugin_desc.version );

    /* Two bits map to 2 states, 0 1 -> 0 and 1 1 -> 1 */
    int i;
    uint8_t nb[3] = {0};

    if (((bb[0][0]&0x55)==0x55) && ((bb[0][1]&0x55)==0x55) && ((bb[0][2]&0x55)==0x55) && ((bb[0][3]&0x55)==0x00)) {
        for (i=0 ; i<3 ; i++) {
            nb[i] |= ((bb[0][i]&0xC0)==0xC0) ? 0x00 : 0x01;
            nb[i] |= ((bb[0][i]&0x30)==0x30) ? 0x00 : 0x02;
            nb[i] |= ((bb[0][i]&0x0C)==0x0C) ? 0x00 : 0x04;
            nb[i] |= ((bb[0][i]&0x03)==0x03) ? 0x00 : 0x08;
        }

        fprintf(stderr, "Remote button event:\n");
        fprintf(stderr, "model   = Waveman Switch Transmitter\n");
        fprintf(stderr, "id      = %c\n", 'A'+nb[0]);
        fprintf(stderr, "channel = %d\n", (nb[1]>>2)+1);
        fprintf(stderr, "button  = %d\n", (nb[1]&3)+1);
        fprintf(stderr, "state   = %s\n", (nb[2]==0xe) ? "on" : "off");
        fprintf(stderr, "%02x %02x %02x\n",nb[0],nb[1],nb[2]);

        return 1;
    }
    return 0;
}

