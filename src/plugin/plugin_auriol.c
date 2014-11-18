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

static int auriol_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

r_device auriol = {
    /* .id             = */ 1,
    /* .name           = */ "Auriol Temperature Sensor",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 750,
    /* .long_limit     = */ 1500,
    /* .reset_limit    = */ 5000,
    /* .json_callback  = */ &auriol_callback,
};

rtl_433_plugin_t plugin =
{
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "environment.temperature",
        .model       = "Auriol Temperature Station",
        .version     = 1
    },
    .r_device_p = &auriol
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

// Callback for Aureol Temperature station compatible sensors
static int auriol_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {
    int temperature_before_dec;
    int temperature_after_dec;
    int16_t temp;
    int sensor_id;
    uint32_t *notnul;
    int i,j;
    time_t timer;
    char buffer[25];
    struct tm* tm_info;

// TODO make DEBUG
    fprintf(stderr, "Called plugin callback for App: %s, type: %s, model: %s, version: %d\n",
        plugin.plugin_desc.application,
        plugin.plugin_desc.type,
        plugin.plugin_desc.model,
        plugin.plugin_desc.version );

    /* FIXME validate the received message better, figure out crc so we can prevent false positives */
    notnul = (uint32_t*)&bb[1][0];
    if ( *notnul != 0 && bb[0][0] == 0 && bb[1][0] != 0 &&
         bb[1][0] == bb[2][0] && bb[2][0] == bb[3][0] && bb[3][0] == bb[4][0] &&
         bb[4][0] == bb[5][0] && bb[5][0] == bb[6][0] && bb[6][0] == bb[7][0] ) {
        for( i = 1; i < 8; i++)
        {
            if( memcmp( &(bb[1]), &(bb[i]), 4 ) != 0 )
            {
                return 0;
            }
        }

        /* Nible 3,4,5 contains 12 bits of temperature
         * The temperature is signed and scaled by 10 */

        temp = (int16_t)((uint16_t)(bb[1][1] << 12) | (bb[1][2] << 4));
        temp = temp >> 4;

        sensor_id = bb[1][0];

        temperature_before_dec = abs(temp / 10);
        temperature_after_dec = abs(temp % 10);

        /* Determine current time and put in a formatted string buffer */
        time(&timer);
        tm_info = localtime(&timer);
        strftime(buffer, 25, "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(stderr, "Auriol Temperature Sensor (Z31743A-TX) ID %x(%d) temp = %s%d.%d time = %s\n",
           sensor_id, sensor_id, temp<0?"-":"", temperature_before_dec, temperature_after_dec,  buffer);
        //fprintf(stderr, "protocol       = Auriol\n");
        //fprintf(stderr, "rid            = %x\n",bb[1][0]);
        //fprintf(stderr, "temp           = %s%d.%d\n",temp<0?"-":"",temperature_before_dec, temperature_after_dec);

        return 1;
    }
    return 0;
}
