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

static int xxxx_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);


r_device devices[] = { 
  {
    /* .id             = */ 1,
    /* .name           = */ "First Name",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 1744/4,
    /* .long_limit     = */ 3500/4,
    /* .reset_limit    = */ 5000/4,
    /* .json_callback  = */ &xxxx_callback,
  },
  {
    /* .id             = */ 4,
    /* .name           = */ "First Name",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 3500/4,
    /* .long_limit     = */ 7000/4,
    /* .reset_limit    = */ 15000/4,
    /* .json_callback  = */ &xxxx_callback,
  }
}; 

rtl_433_plugin_t plugin[] = {
  {
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "environment.temperature",
        .model       = "Model",
        .version     = 1
    },
    .r_device_p = &(devices[0])
  },
  {
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "environment.temperature",
        .model       = "Model",
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


static int xxxx_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {

// TODO remove, not valid for multiple plugins with 1 callback
    
    fprintf(stderr, "Called callback plugin for App: %s, type: %s, model: %s, version: %d\n",
        plugin[0].plugin_desc.application,
        plugin[0].plugin_desc.type,
        plugin[0].plugin_desc.model,
        plugin[0].plugin_desc.version );

}

