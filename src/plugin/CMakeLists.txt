set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

add_definitions(-DUSE_PLUGINS )

add_library(plugin_silvercrest  SHARED  plugin_silvercrest.c)
add_library(plugin_rubicson     SHARED  plugin_rubicson.c)
add_library(plugin_auriol       SHARED  plugin_auriol.c)

set(INSTALL_TARGETS plugin_silvercrest plugin_rubicson plugin_auriol)

install(TARGETS ${INSTALL_TARGETS}
    LIBRARY DESTINATION lib${LIB_SUFFIX}/plugin # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

