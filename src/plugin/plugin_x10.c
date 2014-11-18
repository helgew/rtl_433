#include <stdint.h> // for uint8_t
#include <stdio.h>  // fprintf
#include <string.h> // memcmp
#include <stdlib.h> // abs

#include <time.h>

#include "plugin.h"         // for generic plugin_descriptor
#include "rtl_433_plugin.h" // For application plugin descriptor

/**
 *  A very basic X10 decoder, please see:
 *  http://wiki.pilight.org/X10_RF_Receiver.pdf
 *  http://www.ubasics.com/adam/electronics/cm17.shtml
 *  http://www.idobartana.com/hakb/rf_protocol.htm
 *  http://www.edcheung.com/automa/rf.txt
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

/* TODO: Test with X10 remote */
r_device x10 = {
    /* .id             = */ 16,
    /* .name           = */ "X10-Security",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 150,
    /* .long_limit     = */ 420,
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
void printbytebits( uint8_t byte)
{
    char bitstr[8];
    int i;
    for ( i = 0; i < 8; i++ ) {
        bitstr[7-i] = (byte>>i&0x1)?'1':'0';
    }
    fprintf(stderr, "%2x %s", byte, bitstr);
}

static int x10_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {

// TODO remove
    fprintf(stderr, "Called callback plugin for App: %s, type: %s, model: %s, version: %d\n",
        plugin.plugin_desc.application,
        plugin.plugin_desc.type,
        plugin.plugin_desc.model,
        plugin.plugin_desc.version );

// TODO: remove
    /* validate */
    if (((bb[1][0] + bb[1][1]) == 0xff) && ((bb[1][2] + bb[1][3]) == 0xff ))
    {
        uint8_t byte1, byte2;
        byte1 = bb[1][0];
        byte2 = bb[1][2];

        fprintf(stderr, "X10: id = %02x code=%02x\n", byte1, byte2);
        fprintf(stderr, "byte1: ");
        printbytebits( byte1 );
        fprintf(stderr, "\nbyte2: ");
        printbytebits( byte2 );
        fprintf(stderr, "\n");
        fprintf(stderr, "byte2 & 0x7 %x, byte2 %x\n", byte2 & 0x7, byte2);
        if ( (byte2 & 0x7) == 0x0 ) {
            fprintf(stderr, "on\n");
        }
        else if ( (byte2 & 0x7)  == 0x4 ) {
            fprintf(stderr, "off\n");
        }
        return 1;
    }
    return 0;
}
