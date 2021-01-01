project(gps)

# Source files
file(GLOB_RECURSE gps_SOURCES
        src/*.c
        src/*.cpp)

# Defines the target which is a static library composed by the gps lib’s source files
add_library(${PROJECT_NAME} STATIC ${gps_SOURCES})

# Include the target lib’s inclusion directories by calling:
target_include_directories(${PROJECT_NAME} PUBLIC include PRIVATE src)
# Note that the include folder is PUBLIC and will be used by lib1 and its clients,
# whereas the src folder is PRIVATE and is going to be used just by gps lib.

# Put all libraries into a variable
set(LIBS stb_image tiny_obj_loader)

# Define the link libraries
# PRIVATE, so it’ll just used by the app target and would be hidden if app had clients.
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
