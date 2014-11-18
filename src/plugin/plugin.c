//!
//! plugin.c: Handling of plugins
//!           Discover all plugin's in a given directory
//!

#include "plugin.h"

//    char *application;  //! For what application was this plugin written
//    char *type;         //! What type of plugin (application specific)
//    char *model;        //! The specific model for which the plugin is written
//    int version;        //! The version of the plugin

#include <stdio.h>  // fprintf etc.
#include <stdarg.h> // For varargs
#include <dirent.h> // opendir, readdir
#include <dlfcn.h>  // dlopen
#include <string.h> // strlen
#include <stdlib.h> // malloc, realloc, free


// This is the signature of a plugin initialization function that the plugins
// must export as init_<pluginname>. This function accepts a pointer to a plugin
// manager. Its return value means an error if < 0, success otherwise.
//
typedef void* (*PluginInitFunc)();


static void *load_plugin( const char *plugin_name, const char *plugin_path );

//
// TODO: add 'type' (regex) string so that only plugin's of required type
// are loaded.
//
//! param dirname       char pointer containing a directory name where plugin's should be discovered
//! param plugins       pointer to a an (allocced) array of plugin's found already (NULL if no plugins found yet)
//! return              pointer to a an array of plugin's found (NULL if no plugins found yet)

VISIBLE plugin_array *pm_discover_plugins( const char* dirname, plugin_array *plugins )
{
    char *full_path = NULL;
    struct dirent* direntry = NULL;

    if ( dirname == NULL )
    {
        dirname = ".";  // Use current directory as last resort TODO: use 
    }
    if ( plugins == NULL )
    {
        /* calloc  initializes  the structure to 0, NULL*/
        plugins = calloc( 1, sizeof(plugin_array) );
    }

    if ( plugins == NULL )
    {
        fprintf(stderr, "Could not allocate plugin_array");
        return NULL;
    }


    DIR *dir = opendir(dirname);
    if( ! dir )
    {
        fprintf ( stderr, "Directory '%s' can not be opened, plugin's can not be discovered\n",
            dirname) ;
        return 0;
    }

    while ((direntry = readdir(dir)))
    {
        char *name = direntry->d_name;
        // TODO: have a look at expected extensions for other platforms 
        if (!name || direntry->d_type != DT_REG || ! strstr( name, ".so" )  )
        {
            /* direntry has no name or is not a regular file */
            continue;
        }
        // note size+2 for '/' and '\0'
        size_t max_size = (strlen(dirname)+strlen(name)+2);
        full_path = realloc( full_path, max_size );
        snprintf( full_path, max_size, "%s/%s", dirname, name );
        fprintf ( stderr, "Loading plugin '%s' from '%s' \n", name, full_path ) ;
        /* TODO: store returned plugin in admin */
        void *last_plugin = load_plugin( name, full_path );
        if (last_plugin)
        {
            plugins->count++;
            plugins->plugins = realloc(plugins->plugins, (plugins->count * sizeof(void*)));
            plugins->plugins[plugins->count-1] = last_plugin;
        }
    }
    if ( dir ) closedir( dir );
    if ( full_path ) free( full_path );
//  TODO: ceate real plugin admin
    return plugins;
}

void pm_show_plugins(  plugin_array *plugins )
{
    int i;

    for ( i = 0; i < plugins->count; i++ )
    {
        pm_show_plugin( (plugin_descriptor*)plugins->plugins[i] );
    }
}

void pm_show_plugin (  plugin_descriptor  *plugin )
{
    printf( "Plugin: Application %15s  Type:  %15s  Model: %15s  Version: %4d\n", 
        plugin->application,
        plugin->type,
        plugin->model,
        plugin->version );
}
//! param plugin_name   char pointer containing the name of the plugin
//! param plugin_path   char pointer containing the path where the plugin to be loaded can be found
//! return              a pointer to a plugin_handle for the plugin
static void *load_plugin( const char *plugin_name, const char *plugin_path )
{
    PluginInitFunc mainfunc;
    void *plugin;

    void* libhandle = dlopen( plugin_path, RTLD_LAZY );
    if ( ! libhandle )
    {
        fprintf ( stderr, "Can not open plugin '%s' at '%s' (cause: %s)\n",
            plugin_name, plugin_path, dlerror() );
    }
    else
    {
        printf ( "dlopen plugin '%s' at '%s'\n", plugin_name, plugin_path );
        mainfunc = (PluginInitFunc)dlsym(libhandle, "get_plugin");
    }
    if (!mainfunc) {
        printf("Error loading init function: %s\n", dlerror());
        dlclose(libhandle);
        return NULL;
    }

    plugin = mainfunc();
    if (plugin == NULL) {
        printf("Error: Plugin init function returned NULL\n");
        dlclose(libhandle);
        return NULL;
    }

    printf("Loaded plugin from: '%s'\n", plugin_path);
    return plugin;

}

