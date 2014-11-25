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

/* TODO: Replace the pattern xxxx with the name for your plugin, note that name conflicts don't matter 
 *    as long as the library file for your plugin is unique
 */
static int xxxx_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

/* TODO: set the right PWM decoder, adjust the short, long and reset limits for your plugin */
r_device xxxx = {
    /* .id             = */ 100,
    /* .name           = */ "Your Plugin Name",
    /* .modulation     = */ OOK_PWM_P,
    /* .short_limit    = */ 600/4,
    /* .long_limit     = */ 5000/4,
    /* .reset_limit    = */ 15000/4,
    /* .json_callback  = */ &xxxx_callback,
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
rtl_433_plugin_t plugin =
{
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "xxxx.xxxx",
        .model       = "xxxx",
        .version     = 1
    },
    .r_device_p = &xxxx
};


/* TODO: put your callback function here, remove the template below, or adjust it */
static int xxxx_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {
// MYTODO remove or replace by debug output call
    fprintf(stderr, "Called callback plugin for App: %s, type: %s, model: %s, version: %d\n",
        plugin.plugin_desc.application,
        plugin.plugin_desc.type,
        plugin.plugin_desc.model,
        plugin.plugin_desc.version );

    if ( 1 = 1 ) { // TODO: Add checks for validity of this message for this protocol to the if statement
        // TODO: Add code for extracting usefull values from the message and display them
        // In a later stage dedicated callbacks will be provided to do this in a uniform way
        // accross device sepcific protocol callbacks
        return 1;
    }
    return 0;
}

/* No need to change this function */
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
