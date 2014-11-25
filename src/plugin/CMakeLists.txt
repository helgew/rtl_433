set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

add_definitions(-DUSE_PLUGINS )

add_library(plugin_silvercrest  SHARED  plugin_silvercrest.c)
add_library(plugin_rubicson     SHARED  plugin_rubicson.c)
add_library(plugin_auriol       SHARED  plugin_auriol.c)
add_library(plugin_x10          SHARED  plugin_x10.c)
add_library(plugin_prologue     SHARED  plugin_prologue.c)
add_library(plugin_waveman      SHARED  plugin_waveman.c)
add_library(plugin_em1000       SHARED  plugin_em1000.c)
add_library(plugin_steffen      SHARED  plugin_steffen.c)
add_library(plugin_ws2000       SHARED  plugin_ws2000.c)
add_library(plugin_acurite      SHARED  plugin_acurite.c)

set(INSTALL_TARGETS plugin_silvercrest plugin_rubicson plugin_auriol plugin_x10 plugin_prologue plugin_waveman plugin_em1000 plugin_steffen plugin_ws2000 plugin_acurite)

install(TARGETS ${INSTALL_TARGETS}
    LIBRARY DESTINATION lib${LIB_SUFFIX}/plugin # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

