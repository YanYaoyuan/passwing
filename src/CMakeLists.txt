add_executable(passwing)
add_executable(PassWing::Application ALIAS passwing)

add_subdirectory(app)
add_subdirectory(common)
add_subdirectory(widgets)
add_subdirectory(airfoil)
add_subdirectory(wing)
add_subdirectory(aircraft)
add_subdirectory(propeller)
add_subdirectory(geometry)
add_subdirectory(dynamics)
add_subdirectory(chat)
add_subdirectory(help)

target_sources(passwing PRIVATE
    "${PROJECT_SOURCE_DIR}/resoure/passwing.qrc"
)

if(WIN32)
    target_sources(passwing PRIVATE "${PROJECT_SOURCE_DIR}/win.rc")
endif()

target_include_directories(passwing PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(passwing PRIVATE
    PassWing::Dependencies
    PassWing::ProjectOptions
)

passwing_apply_project_options(passwing)

if(COMMAND vtk_module_autoinit)
    vtk_module_autoinit(TARGETS passwing MODULES ${VTK_LIBRARIES})
endif()

set_target_properties(passwing PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    OUTPUT_NAME PassWing
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    WIN32_EXECUTABLE TRUE
)

install(TARGETS passwing
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    BUNDLE DESTINATION .
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/theoreticalFramework"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/PassWing"
)
