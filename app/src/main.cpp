#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include "Model.hpp"
#include "view_mode.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 view;
glm::mat4 projection;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLuint viewLoc;
GLuint projectionLoc;
GLuint lightDirLoc;
GLuint lightColorLoc;

// camera
gps::Camera myCamera(
        glm::vec3(0.0f, 3.0f, 0.0f),
        glm::vec3(0.0f, 3.0f, -10.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

ViewMode viewMode = ViewMode::SOLID_SMOOTH;


GLboolean pressedKeys[1024];

// models
Model baseScene;
GLfloat angle;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;

// skybox
gps::SkyBox mySkyBox;

// constants
float zNear = 0.1f;
float zFar = 500.0f;
GLfloat cameraSpeed = 0.5f;

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void updateViewMatrix() {
    //obtain view matrix
    view = myCamera.getViewMatrix();

    //send view matrix to shader
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // update the normal matrices of the objects
    baseScene.updateNormalMatrix(view);
}

void updateProjectionMatrix() {
    const float aspect = (float) myWindow.getWindowDimensions().width
            / (float) myWindow.getWindowDimensions().height;

    // create projection matrix
    projection = glm::perspective(
            glm::radians(45.0f),
            aspect,
            zNear,
            zFar
    );

    // send projection matrix to shader
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);

    myWindow.setWindowDimensions({width, height});

    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    updateProjectionMatrix();
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = xpos;
    static double lastY = ypos;

    float deltaX = lastX - xpos;
    float deltaY = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.03f;
    deltaX *= sensitivity;
    deltaY *= sensitivity;

    float yaw = deltaX;
    float pitch = deltaY;

    if (yaw > 89.0f)
        yaw = 89.0f;
    if (yaw < -89.0f)
        yaw = -89.0f;

    myCamera.rotate(pitch, yaw);
    updateViewMatrix();
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        baseScene.updateModelMatrix(model, view);
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for base scene
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        baseScene.updateModelMatrix(model, view);
    }

    if (pressedKeys[GLFW_KEY_V]) {
        nextViewMode(myBasicShader, viewMode);
        pressedKeys[GLFW_KEY_V] = 0;
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initShaders() {
    myBasicShader.loadShader(
            "shaders/basic.vert",
            "shaders/basic.frag",
            "shaders/basic.geom");
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // obtain projection matrix location
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // create and send projection matrix to shader
    updateProjectionMatrix();

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void initModels() {
    baseScene.LoadModel("models/scene/scene.obj");
    baseScene.init(myBasicShader, glm::mat4(1.0f), view);
}

void initSkyBox() {
    std::vector<const GLchar*> faces;
    faces.push_back("textures/skybox/right.tga");
    faces.push_back("textures/skybox/left.tga");
    faces.push_back("textures/skybox/top.tga");
    faces.push_back("textures/skybox/bottom.tga");
    faces.push_back("textures/skybox/back.tga");
    faces.push_back("textures/skybox/front.tga");

    mySkyBox.Load(faces);

    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    // send view matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));


    glm::mat4 projectionSkyBox = glm::perspective(glm::radians(45.0f),
                                  (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                                                  0.1f, 1000.0f);

    // send projection matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projectionSkyBox));
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render the scene

    // draw base scene
    baseScene.Draw(myBasicShader);

    mySkyBox.Draw(skyboxShader, view, projection);
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initShaders();
    initUniforms();
    initModels();
    initSkyBox();
    setViewMode(myBasicShader, viewMode);
    setWindowCallbacks();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
