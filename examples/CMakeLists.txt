foreach(example "example1")
  qt_add_executable(${example} ${example}.cpp)

  qt6_add_qml_module(
    ${example}
    URI
    E${example}
    VERSION
    1.0
    QML_FILES
    "qml/${example}.qml"
    RESOURCE_PREFIX
    "/amarulasolutions.com/imports"
    IMPORT_PATH
    ${CMAKE_BINARY_DIR})

  target_link_libraries(${example} PRIVATE Qt::Gui Qt::Qml Qt::Quick)

endforeach()
