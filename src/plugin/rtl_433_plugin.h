//
//! rtl_433_plugin.h: The definitions shared by the application and the plugins
//

#include "plugin.h"

#define BITBUF_COLS             34
#define BITBUF_ROWS             50

#define DEFAULT_APP_DIRECTORY      "/usr/local"
#define DEFAULT_PLUGIN_DIRECTORY   DEFAULT_APP_DIRECTORY "/lib/plugin"

/* Supported modulation types */
#define     OOK_PWM_D   1   /* Pulses are of the same length, the distance varies */
#define     OOK_PWM_P   2   /* The length of the pulses varies */

/* Function declarations */

typedef int (*callback_t)(uint8_t[BITBUF_ROWS][BITBUF_COLS]);

typedef struct {
    unsigned int    id;
    char            name[256];
    unsigned int    modulation;
    unsigned int    short_limit;
    unsigned int    long_limit;
    unsigned int    reset_limit;
    int     (*json_callback)(uint8_t bits_buffer[BITBUF_ROWS][BITBUF_COLS]) ;
} r_device;

typedef struct rtl_433_plugin_t {
    plugin_descriptor  plugin_desc;
    r_device          *r_device_p;
} rtl_433_plugin_t;

extern void *get_plugin();

