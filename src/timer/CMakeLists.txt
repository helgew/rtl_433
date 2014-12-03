set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")


add_library(timer  SHARED  timer.c)

set(INSTALL_TARGETS timer test_timer)

add_executable(test_timer test_timer.c)

install(TARGETS ${INSTALL_TARGETS}
    LIBRARY DESTINATION lib${LIB_SUFFIX}/timer # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

