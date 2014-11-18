#include <stdint.h> // for uint8_t
#include <stdio.h>  // fprintf
#include <string.h> // memcmp
#include <stdlib.h> // abs

#include <time.h>

#include "plugin.h"         // for generic plugin_descriptor
#include "rtl_433_plugin.h" // For application plugin descriptor

/**
 *  Decoder for em1000 energy monitor from ELV
 *  http://fhz4linux.info/tiki-index.php?page=EM+Protocol
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
static int em1000_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]);

r_device elv_em1000 = {
    /* .id             = */ 7,
    /* .name           = */ "ELV EM 1000",
    /* .modulation     = */ OOK_PWM_D,
    /* .short_limit    = */ 750/4,
    /* .long_limit     = */ 7250/4,
    /* .reset_limit    = */ 30000/4,
    /* .json_callback  = */ &em1000_callback,
};


rtl_433_plugin_t plugin =
{
    .plugin_desc = {
        .application = "rtl_433",
        .type        = "energy.counter",
        .model       = "elv_em1000",
        .version     = 1
    },
    .r_device_p = &elv_em1000
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

uint16_t AD_POP(uint8_t bb[BITBUF_COLS], uint8_t bits, uint8_t bit) {
    uint16_t val = 0;
    uint8_t i, byte_no, bit_no;
    for (i=0;i<bits;i++) {
        byte_no=   (bit+i)/8 ;
        bit_no =7-((bit+i)%8);
        if (bb[byte_no]&(1<<bit_no)) val = val | (1<<i);
    }
    return val;
}

static int em1000_callback(uint8_t bb[BITBUF_ROWS][BITBUF_COLS]) {
    // based on fs20.c
    uint8_t dec[10];
    uint8_t bytes=0;
    uint8_t bit=18; // preamble
    uint8_t bb_p[14];
    char* types[] = {"S", "?", "GZ"};
    uint8_t checksum_calculated = 0;
    uint8_t i;
        uint8_t stopbit;
        uint8_t checksum_received;

// TODO remove or replace by debug output call
    fprintf(stderr, "Called callback plugin for App: %s, type: %s, model: %s, version: %d\n",
        plugin.plugin_desc.application,
        plugin.plugin_desc.type,
        plugin.plugin_desc.model,
        plugin.plugin_desc.version );

    // check and combine the 3 repetitions
    for (i = 0; i < 14; i++) {
        if(bb[0][i]==bb[1][i] || bb[0][i]==bb[2][i]) bb_p[i]=bb[0][i];
        else if(bb[1][i]==bb[2][i])                  bb_p[i]=bb[1][i];
        else return 0;
    }

    // read 9 bytes with stopbit ...
    for (i = 0; i < 9; i++) {
        dec[i] = AD_POP (bb_p, 8, bit); bit+=8;
        stopbit=AD_POP (bb_p, 1, bit); bit+=1;
        if (!stopbit) {
//            fprintf(stderr, "!stopbit: %i\n", i);
            return 0;
        }
        checksum_calculated ^= dec[i];
        bytes++;
    }

    // Read checksum
    checksum_received = AD_POP (bb_p, 8, bit); bit+=8;
    if (checksum_received != checksum_calculated) {
//        fprintf(stderr, "checksum_received != checksum_calculated: %d %d\n", checksum_received, checksum_calculated);
        return 0;
    }

//for (i = 0; i < bytes; i++) fprintf(stderr, "%02X ", dec[i]); fprintf(stderr, "\n");

    // based on 15_CUL_EM.pm
    fprintf(stderr, "Energy sensor event:\n");
    fprintf(stderr, "protocol      = ELV EM 1000\n");
    fprintf(stderr, "type          = EM 1000-%s\n",dec[0]>=1&&dec[0]<=3?types[dec[0]-1]:"?");
    fprintf(stderr, "code          = %d\n",dec[1]);
    fprintf(stderr, "seqno         = %d\n",dec[2]);
    fprintf(stderr, "total cnt     = %d\n",dec[3]|dec[4]<<8);
    fprintf(stderr, "current cnt   = %d\n",dec[5]|dec[6]<<8);
    fprintf(stderr, "peak cnt      = %d\n",dec[7]|dec[8]<<8);

    return 1;
}

