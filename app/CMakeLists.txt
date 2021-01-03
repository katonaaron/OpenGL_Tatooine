
# Add source files
file(GLOB_RECURSE SOURCE_FILES
        src/*.c
        src/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
        src/*.h
        src/*.hpp)

# Define the include DIRs
include_directories(
        src
        include
)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})



# Libraries

# Libraries

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
find_package(glfw3 3.3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

# GLEW
find_package(GLEW REQUIRED)
message(STATUS "GLEW included at ${GLEW_INCLUDE_DIR}")

# STB_IMAGE
#add_library(STB_IMAGE "thirdparty/stb_image.cpp")
#
## TINY_OBJ_LOADER
#add_library(TINY_OBJ_LOADER "thirdparty/tiny_obj_loader.cpp")

# Put all libraries into a variable
set(LIBS OpenGL::GL glfw GLEW::GLEW gps stb_image tiny_obj_loader)

# Define the link libraries
# PRIVATE, so it’ll just used by the app target and would be hidden if app had clients.
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})




# Shaders, models, textures

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
        COMMENT "Creating symlink to shader directory" VERBATIM
)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/models $<TARGET_FILE_DIR:${PROJECT_NAME}>/models
        COMMENT "Creating symlink to model directory" VERBATIM
)
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/textures $<TARGET_FILE_DIR:${PROJECT_NAME}>/textures
        COMMENT "Creating symlink to model directory" VERBATIM
)
