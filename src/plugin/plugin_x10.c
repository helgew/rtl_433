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
    static int returned = 0;
    if ( returned == 0)
    {
        returned = 1;
        return &plugin;
    }
    return NULL;

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

    char *x10_function[] = {
        "All Units Off",
        "All Lights On",
        "On",
        "Off",
        "Dim",
        "Bright",
        "All Lights Off",
        "Extended Code",
        "Hail Request",
        "Hail Acknowledge",
        "Pre-set Dim (1)",
        "Pre-set Dim (2)",
        "Extended Data Transfer",
        "Status On",
        "Status Off",
        "Status Request"
    };

    const unsigned long HouseCode[] =
    {
      0x6000, /* a */
      0x7000, /* b */
      0x4000, /* c */
      0x5000, /* d */
      0x8000, /* e */
      0x9000, /* f */
      0xa000, /* g */
      0xb000, /* h */
      0xe000, /* i */
      0xf000, /* j */
      0xc000, /* k */
      0xd000, /* l */
      0x0000, /* m */
      0x1000, /* n */
      0x2000, /* o */
      0x3000  /* p */
    } ;

    const int maxHouseCode = sizeof(HouseCode) / sizeof(HouseCode[0]) ;

    const unsigned long UnitCode[] =
    {
      0x0000, /* 1 */
      0x0010, /* 2 */
      0x0008, /* 3 */
      0x0018, /* 4 */
      0x0040, /* 5 */
      0x0050, /* 6 */
      0x0048, /* 7 */
      0x0058, /* 8 */
      0x0400, /* 9 */
      0x0410, /* 10 */
      0x0408, /* 11 */
      0x0400, /* 12 */
      0x0440, /* 13 */
      0x0450, /* 14 */
      0x0448, /* 15 */
      0x0458  /* 16 */
    } ;
// TODO: remove
    /* validate */
    if (((bb[1][0] + bb[1][1]) == 0xff) && ((bb[1][2] + bb[1][3]) == 0xff ))
    {
        uint8_t byte1, byte2;
        byte1 = bb[1][0];
        byte2 = bb[1][2];
        unsigned long fullcode = byte1 << 8 | byte2;

        fprintf(stderr, "X10: id = %02x code=%02x\n", byte1, byte2);
        fprintf(stderr, "X10: long id = %02lx \n", fullcode );
        fprintf(stderr, "byte1: ");
        printbytebits( byte1 );
        fprintf(stderr, "\nbyte2: ");
        printbytebits( byte2 );
        fprintf(stderr, "\n");
        if ( (byte2 & 0b00100000) ) {
            fprintf(stderr, "off\n");
        }
        else if( (byte2 == 0b10001000) ) {
            fprintf(stderr, "bright\n");
        }
        else if( (byte2 == 0b10011000) ) {
            fprintf(stderr, "dim\n");
        }
        else {
            fprintf(stderr, "on\n");
        }
        return 1;
    }
    return 0;
}
