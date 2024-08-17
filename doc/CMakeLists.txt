find_package(Doxygen)

if (DOXYGEN_FOUND)
    # Download custom CSS files
    set(EXTRA_STRYLESHEET0 "${CMAKE_CURRENT_BINARY_DIR}/doxygen/doxygen-awesome.css")
    set(EXTRA_STRYLESHEET1 "${CMAKE_CURRENT_BINARY_DIR}/doxygen/doxygen-awesome-sidebar-only.css")
    
    file(DOWNLOAD https://raw.githubusercontent.com/jothepro/doxygen-awesome-css/v2.3.1/doxygen-awesome.css
    ${EXTRA_STRYLESHEET0})
    file(DOWNLOAD https://raw.githubusercontent.com/jothepro/doxygen-awesome-css/v2.3.1/doxygen-awesome-sidebar-only.css
    ${EXTRA_STRYLESHEET1})

    # Set project-specific Doxygen settings
    set(DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set(DOXYGEN_GENERATE_HTML "YES")
    set(DOXYGEN_HTML_OUTPUT "${CMAKE_BINARY_DIR}/html")
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    list(APPEND DOXYGEN_HTML_EXTRA_STYLESHEET
        ${EXTRA_STRYLESHEET0}
        ${EXTRA_STRYLESHEET1})

    doxygen_add_docs(documentation "${CMAKE_SOURCE_DIR}/include" COMMENT "Generating documentation")
else ()
    message(WARNING "Doxygen not found, unable to generate documentation")
endif ()
