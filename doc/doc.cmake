find_package(Doxygen)

if(${DOXYGEN_FOUND})
    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/stuff)
    set(DOXYGEN_HTML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.cfg.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif(${DOXYGEN_FOUND})
