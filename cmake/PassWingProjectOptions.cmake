include_guard(GLOBAL)

add_library(passwing_project_options INTERFACE)
add_library(PassWing::ProjectOptions ALIAS passwing_project_options)

target_compile_features(passwing_project_options INTERFACE cxx_std_17)

if(MSVC)
    target_compile_definitions(passwing_project_options INTERFACE
        _USE_MATH_DEFINES
    )
    target_compile_options(passwing_project_options INTERFACE
        /permissive-
        /utf-8
        /W4
    )
    if(PASSWING_WARNINGS_AS_ERRORS)
        target_compile_options(passwing_project_options INTERFACE /WX)
    endif()
else()
    target_compile_options(passwing_project_options INTERFACE
        -Wall
        -Wextra
        -Wpedantic
    )
    if(PASSWING_WARNINGS_AS_ERRORS)
        target_compile_options(passwing_project_options INTERFACE -Werror)
    endif()
endif()

if(PASSWING_ENABLE_CLANG_TIDY)
    find_program(PASSWING_CLANG_TIDY_EXECUTABLE NAMES clang-tidy REQUIRED)
endif()

function(passwing_apply_project_options target_name)
    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
    )

    if(PASSWING_ENABLE_CLANG_TIDY)
        set_target_properties(${target_name} PROPERTIES
            CXX_CLANG_TIDY "${PASSWING_CLANG_TIDY_EXECUTABLE}"
        )
    endif()
endfunction()
