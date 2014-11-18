#include <stdint.h> // for uint8_t
#include <stdio.h>  // fprintf
#include <string.h> // memcmp
#include <stdlib.h> // abs

#include <time.h>

#include "plugin.h"         // for generic plugin_descriptor
#include "rtl_433_plugin.h" // For application plugin descriptor


/** Prologue sensor protocol
 *
 * the sensor sends 36 bits 7 times, before the first packet there is a pulse sent
 * the packets are pwm modulated
 *
 * the data is grouped in 9 nibles
 * [id0] [rid0] [rid1] [data0] [temp0] [temp1] [temp2] [humi0] [humi1]
 *
 * id0 is always 1001,9
 * rid is a random id that is generated when the sensor starts, could include battery status
 * the same batteries often generate the same id
 * data(3) is 0 the battery status, 1 ok, 0 low, first reading always say low
 * data(2) is 1 when the sensor sends a reading when pressing the button on the sensor
 * data(1,0)+1 forms the channel number that can be set by the sensor (1-3)
 * temp is 12 bit signed scaled by 10
 * humi0 is always 1100,c if no humidity sensor is available
 * humi1 is always 1100,c if no humidity sensor is available
 *
 * The sensor can be bought at Clas Ohlson
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

static int prologue_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

// TODO: generic: fix the .id clash issue
r_device prologue = {
    /* .id             = */ 2,
    /* .name           = */ "Prologue Temperature Sensor",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 3500/4,
    /* .long_limit     = */ 7000/4,
    /* .reset_limit    = */ 15000/4,
    /* .json_callback  = */ &prologue_callback,
};


rtl_433_plugin_t plugin =
{
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "environment.weather.*",
        .model       = "several",
        .version     = 1
    },
    .r_device_p = &prologue
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

static int prologue_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {
    int rid;
    int16_t temp2;
// TODO remove or replace by debug output call
    fprintf(stderr, "Called callback plugin for App: %s, type: %s, model: %s, version: %d\n",
        plugin.plugin_desc.application,
        plugin.plugin_desc.type,
        plugin.plugin_desc.model,
        plugin.plugin_desc.version );


    /* FIXME validate the received message better */
    if (((bb[1][0]&0xF0) == 0x90 && (bb[2][0]&0xF0) == 0x90 && (bb[3][0]&0xF0) == 0x90 && (bb[4][0]&0xF0) == 0x90 &&
        (bb[5][0]&0xF0) == 0x90 && (bb[6][0]&0xF0) == 0x90) ||
        ((bb[1][0]&0xF0) == 0x50 && (bb[2][0]&0xF0) == 0x50 && (bb[3][0]&0xF0) == 0x50 && (bb[4][0]&0xF0) == 0x50)) {

        /* Prologue sensor */
        temp2 = (int16_t)((uint16_t)(bb[1][2] << 8) | (bb[1][3]&0xF0));
        temp2 = temp2 >> 4;
        fprintf(stderr, "Sensor temperature event:\n");
        fprintf(stderr, "protocol      = Prologue\n");
        fprintf(stderr, "button        = %d\n",bb[1][1]&0x04?1:0);
        fprintf(stderr, "battery       = %s\n",bb[1][1]&0x08?"Ok":"Low");
        fprintf(stderr, "temp          = %s%d.%d\n",temp2<0?"-":"",abs((int16_t)temp2/10),abs((int16_t)temp2%10));
        fprintf(stderr, "humidity      = %d\n", ((bb[1][3]&0x0F)<<4)|(bb[1][4]>>4));
        fprintf(stderr, "channel       = %d\n",(bb[1][1]&0x03)+1);
        fprintf(stderr, "id            = %d\n",(bb[1][0]&0xF0)>>4);
        rid = ((bb[1][0]&0x0F)<<4)|(bb[1][1]&0xF0)>>4;
        fprintf(stderr, "rid           = %d\n", rid);
        fprintf(stderr, "hrid          = %02x\n", rid);

        fprintf(stderr, "%02x %02x %02x %02x %02x\n",bb[1][0],bb[1][1],bb[1][2],bb[1][3],bb[1][4]);

        // TODO: remove 
        //if (debug_output)
            //debug_callback(bb);

        return 1;
    }
    return 0;
}


