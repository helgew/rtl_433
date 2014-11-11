#include <stdint.h> // for uint8_t
#include <stdio.h>  // fprintf
#include <string.h> // memcmp
#include <stdlib.h> // abs

#include <time.h>

#include "plugin.h"         // for generic plugin_descriptor
#include "rtl_433_plugin.h" // For application plugin descriptor

/**
 *  A very basic X10 decoder, please see http://www.ubasics.com/adam/electronics/cm17.shtml
 *  or http://www.idobartana.com/hakb/rf_protocol.htm
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

static int x10_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

r_device x10 = {
    /* .id             = */ 16,
    /* .name           = */ "X10-Security",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 100,
    /* .long_limit     = */ 300,
    /* .reset_limit    = */ 10000/4,
    /* .json_callback  = */ &x10_callback,
};

rtl_433_plugin_t plugin =
{
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "buttons.x10",
        .model       = "x10",
        .version     = 1
    },
    .r_device_p = &x10
};

VISIBLE extern void *get_plugin()
{
    return &plugin;
}
static int x10_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {

// TODO remove
    fprintf(stderr, "Called callback plugin for App: %s, type: %s, model: %s, version: %d\n",
        plugin.plugin_desc.application,
        plugin.plugin_desc.type,
        plugin.plugin_desc.model,
        plugin.plugin_desc.version );

// TODO: remove
fprintf(stderr, "%x %x %x %x\n", bb[1][0], bb[1][1], bb[1][2], bb[1][3] );
    /* validate */
    if ((bb[1][0]^0x0f)  == bb[1][1] && (bb[1][2]^0xff) == bb[1][3])
    {
        fprintf(stderr, "X10SEC: id = %02x%02x code=%02x",bb[1][0],bb[1][4],bb[1][2]);
        return 1;
    }
    return 0;
}
