#include <stdint.h> // for uint8_t
#include <stdio.h>  // fprintf
#include <string.h> // memcmp
#include <stdlib.h> // abs

#include <time.h>

#include "plugin.h"         // for generic plugin_descriptor
#include "rtl_433_plugin.h" // For application plugin descriptor

/**
 *  TODO: Provide information about the protocol and compatible transmitter devices here
 *  Add references (to hopefully permanent URL's) where more detailed info can be found
 *  Note that this info should be picked up by Doxygen to provide documentation for this module
 */

/* Note for developers: add your callback to the CMakeLists.txt file in this directory
 *    then 'cmake' and 'make' the project
 * This is the template for a plugin that handles a single device type with its unique timing limits
 *    if other devices use the same 'protocol' (i.e. message format) so that the callback function
 *    can be used for other devices but with other timings, or another PWM decoder, then use the
 *    plugin_multiple.c template in this directory.
 *    Also if you want to provide multiple related plugins from within a single shared object you can use
 *    the plugin_multiple.c template
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

static int xxxx_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

/* TODO: set the right PWM decoder, adjust the short, long and reset limits for your plugin
 * NOTE: The current timings are based on a sampling rate of 250000 Hz
 * (was originally 10240000 Hz? hence the divide by 4)
 * HOWEVER: we plan on changing this to a unit of usecs in the future, this will make the
 * limits independent of the sampling rate used and allows addaption of the sampling rate
 * This still has to be implemented, all plugins that are part of this repository will be adapted
 */


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

/* TODO set 'type' and 'model' so that it can be used by users to select your plugin, or multiple plugins using wildcards / regexps,
 * A first proposal for valid 'type' values:
 * environment.weather: for weather stations containing multiple sensors
 * environment.temperature: for temperature sensors
 * environment.gas.* for gas sensors, i.e:
 * environment.gas.co: Carbon monoxide sensor
 * environment.gas.co2: Carbon dioxide sensor
 * environment.smoke: Smoke detector
 * remote.button: for button events on remotes (or doorbells)
 * The above 'taxanomy' is a first try, please comment on it if you can come up with a better definition
 *
 * For model just use the brand and model of your sensor / remote / device, off course a plugin may be valid for
 * multiple brands or models, but we need to have some indication of its identity
 */

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

