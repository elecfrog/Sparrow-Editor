# set up default cmake build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# find OpenGL 
find_package(OpenGL 4.5 REQUIRED)

if (OPENGL_FOUND)
    message("Found OpenGL")
else()
    message("Not Found OpenGL")
endif()

file(COPY ${SparrowResourcesDir}
DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message("SparrowLuaLib" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
file(COPY ${SparrowLuaLib}
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_subdirectory(core)
add_subdirectory(examples)
add_subdirectory(ScriptableGame)
add_subdirectory(editor)