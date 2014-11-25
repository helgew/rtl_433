#include <stdint.h> // for uint8_t
#include <stdio.h>  // fprintf
#include <string.h> // memcmp
#include <stdlib.h> // abs

#include <time.h>

#include "plugin.h"         // for generic plugin_descriptor
#include "rtl_433_plugin.h" // For application plugin descriptor

/* from plugin.h: */

//
// typedef struct plugin_descriptor_t {
//     char *application;  //! For what application was this plugin written
//     char *type;         //! What type of plugin (application specific)
//     char *model;        //! The specific model for which the plugin is written
//     int version;        //! The version of the plugin
// } plugin_descriptor;

/* Function declarations */

static int silvercrest_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);


r_device devices[] = {
{
    /* .id             = */ 3,
    /* .name           = */ "Silvercrest Remote Control",
    /* .modulation     = */ OOK_PWM_P,
    /* .short_limit    = */ 600/4,
    /* .long_limit     = */ 5000/4,
    /* .reset_limit    = */ 15000/4,
    /* .json_callback  = */ &silvercrest_callback,
},
{
    /* .id             = */ 5,
    /* .name           = */ "HX2262 Window/Door sensor",
    /* .modulation     = */ OOK_PWM_P,
    /* .short_limit    = */ 1300/4,
    /* .long_limit     = */ 10000/4,
    /* .reset_limit    = */ 40000/4,
    /* .json_callback  = */ &silvercrest_callback,
}
};


rtl_433_plugin_t plugin[] = {
  {
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "remote.buttons",
        .model       = "Silvervrest Remote",
        .version     = 1
    },
    .r_device_p = &(devices[0])
  },
  {
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "remote.buttons",
        .model       = "HX2262 Window Door Sensor",
        .version     = 1
    },
    .r_device_p = &(devices[1])
  }
};


VISIBLE extern void *get_plugin()
{
    static int cnt = 0;
    // TODO: return each record in order
    if ( cnt < 2 )
    {
        return &(plugin[cnt++]);
    }
    return NULL;
}

static int silvercrest_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {

// TODO remove
    fprintf(stderr, "App: %s, type: %s, model: %s, version: %d\n",
        plugin[0].plugin_desc.application,
        plugin[0].plugin_desc.type,
        plugin[0].plugin_desc.model,
        plugin[0].plugin_desc.version );

    /* FIXME validate the received message better */
    if (bb[1][0] == 0xF8 &&
        bb[2][0] == 0xF8 &&
        bb[3][0] == 0xF8 &&
        bb[4][0] == 0xF8 &&
        bb[1][1] == 0x4d &&
        bb[2][1] == 0x4d &&
        bb[3][1] == 0x4d &&
        bb[4][1] == 0x4d) {
        /* Pretty sure this is a Silvercrest remote */
        fprintf(stderr, "Remote button event:\n");
        fprintf(stderr, "model = Silvercrest\n");
        fprintf(stderr, "%02x %02x %02x %02x %02x\n",bb[1][0],bb[0][1],bb[0][2],bb[0][3],bb[0][4]);

// TODO: see how to handle this
//        if (debug_output)
//            debug_callback(bb);

        return 1;
    }
    return 0;
}

