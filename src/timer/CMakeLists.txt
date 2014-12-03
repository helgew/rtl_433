set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")


add_library(timer  SHARED  timer.c)
add_library(stimer  STATIC  timer.c)

set(INSTALL_TARGETS timer test_timer)

add_executable(test_timer test_timer.c)
target_link_libraries(test_timer stimer)

install(TARGETS ${INSTALL_TARGETS}
    LIBRARY DESTINATION lib${LIB_SUFFIX}/timer # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

