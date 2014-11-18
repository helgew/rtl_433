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


int load_plugins ( const char *plugin_name, const char *plugin_path, plugin_array *plugins );

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
        fprintf ( stderr, "Loading plugins from '%s' \n", full_path ) ;
        /* TODO: store returned plugin in admin */
        int nr_plugins = load_plugins( name, full_path, plugins );
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
//! param plugins       pointer to a plugin array to which to add the plugins in the plugin library file
//! return              the number of plugins added to the plugin array
int load_plugins( const char *plugin_name, const char *plugin_path, plugin_array *plugins )
{ 
    int nr_plugins=0;
    PluginInitFunc get_plugin_func = NULL;
    plugin_descriptor *plugin = NULL;

    void* libhandle = dlopen( plugin_path, RTLD_LAZY );
    if ( ! libhandle )
    {
        fprintf ( stderr, "Can not open plugin '%s' at '%s' (cause: %s)\n",
            plugin_name, plugin_path, dlerror() );
        return 0;
    }

    // printf ( "dlopen plugin '%s' at '%s'\n", plugin_name, plugin_path );
    get_plugin_func = (PluginInitFunc)dlsym(libhandle, "get_plugin");

    if ( ! get_plugin_func ) {
        fprintf( stderr, "Error loading 'get_plugin' function from %s: %s\n", plugin_path, dlerror());
        dlclose(libhandle);
        return 0;
    }

    while ( (plugin = get_plugin_func()) )
    {
        nr_plugins++;
        plugins->count++;
        plugins->plugins = realloc(plugins->plugins, (plugins->count * sizeof(void*)));
        plugins->plugins[plugins->count-1] = plugin;
        fprintf( stderr, "Loading plugin %s\n", plugin->model );
    }

    if ( (nr_plugins == 0) ) {
        printf("Error: No plugins returned by get_plugin for %s\n", plugin_path);
        dlclose(libhandle);
        return 0;
    }

    // TODO: make this a debug statement
    printf("Loaded %d plugin from: '%s'\n", nr_plugins, plugin_path);
    return nr_plugins;

}

