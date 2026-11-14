cmake_minimum_required(VERSION 3.15)
project(Pesma VERSION 0.1.0 LANGUAGES C)

add_library(pesma SHARED
    src/p_buffer.c
    src/p_file.c
    src/p_network.c
    src/p_utils.c
)
target_include_directories(pesma PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>)
target_compile_features(pesma PUBLIC c_std_99)
target_compile_definitions(pesma PRIVATE _POSIX_C_SOURCE=200809L)

include(GNUInstallDirs)
install(TARGETS pesma EXPORT PesmaTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES inc/pesma.h inc/pesma_internal.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT PesmaTargets FILE PesmaTargets.cmake
    NAMESPACE Pesma:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Pesma)
