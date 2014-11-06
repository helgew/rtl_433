//
//! plugin.h: include file for external interface of plugin handling
//

#ifndef PLUGIN_H
#define PLUGIN_H

#define VISIBLE __attribute__ ((visibility ("default")))

typedef struct plugin_array_t {
    void **plugins ;
    int  count;
}  plugin_array;

typedef struct plugin_descriptor_t {
    char *application;  //! For what application was this plugin written
    char *type;         //! What type of plugin (application specific)
    char *model;        //! The specific model for which the plugin is written
    int version;        //! The version of the plugin
} plugin_descriptor;

plugin_array *pm_discover_plugins( const char* dirname, plugin_array *plugins );


VISIBLE void pm_show_plugins(  plugin_array *plugins );

VISIBLE void pm_show_plugin (  plugin_descriptor  *plugin );

#endif
