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

static int rubicson_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

// TODO: Better solution for .id */
// TODO Rubicson callback has multiple r_device entries tech_line_fws_500

r_device devices[] = { 
  {
    /* .id             = */ 1,
    /* .name           = */ "Rubicson Temperature Sensor",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 1744/4,
    /* .long_limit     = */ 3500/4,
    /* .reset_limit    = */ 5000/4,
    /* .json_callback  = */ &rubicson_callback,
  },
  {
    /* .id             = */ 4,
    /* .name           = */ "Tech Line FWS-500 Sensor",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 3500/4,
    /* .long_limit     = */ 7000/4,
    /* .reset_limit    = */ 15000/4,
    /* .json_callback  = */ &rubicson_callback,
  }
}; 

rtl_433_plugin_t plugin[] = {
  {
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "environment.temperature",
        .model       = "Rubicson Temperature Sensor",
        .version     = 1
    },
    .callback_p = rubicson_callback,
    .r_device_p = &(devices[0])
  },
  {
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "environment.temperature",
        .model       = "Tech Line FWS-500 Sensor",
        .version     = 1
    },
    .callback_p = rubicson_callback,
    .r_device_p = &(devices[1])
  }
};

VISIBLE extern void *get_plugin()
{
    // TODO: return each record in order
    return &(plugin[1]);
}
static int rubicson_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {
    int temperature_before_dec;
    int temperature_after_dec;
    int16_t temp;


// TODO remove, not valid for multiple plugins with 1 callback
    
    fprintf(stderr, "App: %s, type: %s, model: %s, version: %d\n",
        plugin[1].plugin_desc.application,
        plugin[1].plugin_desc.type,
        plugin[1].plugin_desc.model,
        plugin[1].plugin_desc.version );

    /* FIXME validate the received message better, figure out crc */
    if (bb[1][0] == bb[2][0] && bb[2][0] == bb[3][0] && bb[3][0] == bb[4][0] &&
        bb[4][0] == bb[5][0] && bb[5][0] == bb[6][0] && bb[6][0] == bb[7][0] && bb[7][0] == bb[8][0] &&
        bb[8][0] == bb[9][0] && (bb[5][0] != 0 && bb[5][1] != 0 && bb[5][2] != 0)) {

        /* Nible 3,4,5 contains 12 bits of temperature
         * The temerature is signed and scaled by 10 */
        temp = (int16_t)((uint16_t)(bb[0][1] << 12) | (bb[0][2] << 4));
        temp = temp >> 4;

        temperature_before_dec = abs(temp / 10);
        temperature_after_dec = abs(temp % 10);

        fprintf(stderr, "Sensor temperature event:\n");
        fprintf(stderr, "protocol       = Rubicson/Auriol\n");
        fprintf(stderr, "rid            = %x\n",bb[0][0]);
        fprintf(stderr, "temp           = %s%d.%d\n",temp<0?"-":"",temperature_before_dec, temperature_after_dec);
        fprintf(stderr, "%02x %02x %02x %02x %02x\n",bb[1][0],bb[0][1],bb[0][2],bb[0][3],bb[0][4]);

        return 1;
    }
    return 0;
}

