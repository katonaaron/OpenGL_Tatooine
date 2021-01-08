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
#include "Sun.hpp"
#include "lights.hpp"

#include <iostream>
#include <memory>

// window
gps::Window myWindow;

// matrices
glm::mat4 view;
glm::mat4 projection;

// sun
Sun sun;
glm::vec3 sunRotateAxis(1.0f, 1.0f, 1.0f);
GLfloat sunRadius = 300.0f;
GLfloat sunAngle = 180.0f;
GLfloat sunScale= 5.0f;
DirLight sunLight = {
        .direction = glm::vec3(),
        .ambient =  glm::vec3(1.0f, 1.0f, 1.0f), //white light
        .diffuse =  glm::vec3(1.0f, 1.0f, 1.0f), //white light
        .specular =  glm::vec3(1.0f, 1.0f, 1.0f), //white light
};
DirLight nightLight = {
        .direction = glm::vec3(),
        .ambient =  glm::vec3(0.0f, 0.0f, 0.0f), //white light
        .diffuse =  glm::vec3(0.0f, 0.0f, 0.0f), //white light
        .specular =  glm::vec3(0.0f, 0.0f, 0.0f), //white light
};

// Day or Night
bool isDay = true;

// shader uniform locations
GLuint lightDirLoc;
GLuint lightColorLoc;

// camera
gps::Camera myCamera(
        glm::vec3(0.0f, 2.0f, 0.0f),
        glm::vec3(0.0f, 2.0f, -10.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

ViewMode viewMode = ViewMode::SOLID_SMOOTH;


GLboolean pressedKeys[1024];

// models
Model baseScene;
Model babyYoda;
Model ship;
std::vector<std::shared_ptr<Model>> models;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader simpleShader;
std::vector<std::shared_ptr<gps::Shader>> shaders;
std::vector<std::shared_ptr<gps::Shader>> lightingShaders;

// skybox
gps::SkyBox skyBoxDay;
gps::SkyBox skyBoxNight;

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

void updateViewMatrix(bool updateNormals = true) {
    //obtain view matrix
    view = myCamera.getViewMatrix();

    //send view matrix to shaders
    for (const std::shared_ptr<gps::Shader> &shader : shaders) {
        shader->useShaderProgram();
        shader->setUniform("view", view);
    }

    if (updateNormals) {
        // update the normal matrices of the models
        for (const std::shared_ptr<Model> &model : models) {
            model->updateNormalMatrix(view);
        }
    }
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

    //send projection matrix to shaders
    for (const std::shared_ptr<gps::Shader> &shader : shaders) {
        shader->useShaderProgram();
        shader->setUniform("projection", projection);
    }
}

void updateSunlight() {
    static const glm::vec3 axisY = glm::vec3(0.0f, 1.0f, 0.0f);

    sunLight.direction = glm::normalize(sun.getPosition()); // -(0, 0, 0). The vector from the origin to the center of the object.

    isDay = angleBetween(axisY, sunLight.direction) < 80.0f;

    // send the directional sunlight data to the shaders which depend on it
    for(const auto& shader : lightingShaders) {
        if(angleBetween(axisY, sunLight.direction) < 90.0f) {
            sendDirLight(sunLight, *shader);
        } else {
            sendDirLight(nightLight, *shader);
        }
    }
}

void rotateSun(float angle) {
    sun.rotate(angle);
    updateSunlight();
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
        if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
            sun.scale(-1.0f);
        } else {
            rotateSun(-1.0f);
        }
    }

    if (pressedKeys[GLFW_KEY_E]) {
        if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
            sun.scale(1.0f);
        } else {
            rotateSun(1.0f);
        }
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
    simpleShader.loadShader("shaders/simpleTex.vert",
                            "shaders/simpleTex.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert",
                            "shaders/skyboxShader.frag");

    // Add shaders to the list of shaders that require view and projection matrices
    shaders.push_back(std::make_shared<gps::Shader>(myBasicShader));
    shaders.push_back(std::make_shared<gps::Shader>(simpleShader));

    // Update the view and projection matrices of the dependent shaders
    updateViewMatrix(false);
    updateProjectionMatrix();

    // Add shaders to the list of shaders that require data of lights (directional or positional)
    lightingShaders.push_back(std::make_shared<gps::Shader>(myBasicShader));
}

void initLights() {
    sun.LoadModel("models/sun/sun.obj");
    sun.init(sunRotateAxis, sunRadius, sunScale, sunAngle);
    updateSunlight();
}

void initModels() {
    baseScene.LoadModel("models/scene/scene.obj");
    baseScene.init(myBasicShader, glm::mat4(1.0f), view);
    babyYoda.LoadModel("models/baby_yoda/baby_yoda.obj");
    babyYoda.init(myBasicShader, glm::mat4(1.0f), view);
    ship.LoadModel("models/ship/ship.obj");
    ship.init(myBasicShader, glm::mat4(1.0f), view);

    models.push_back(std::make_shared<Model>(baseScene));
    models.push_back(std::make_shared<Model>(babyYoda));
    models.push_back(std::make_shared<Model>(ship));
}

void initSkyBox() {
    std::vector<const GLchar*> dayFaces;
    dayFaces.push_back("textures/skybox-day/right.tga");
    dayFaces.push_back("textures/skybox-day/left.tga");
    dayFaces.push_back("textures/skybox-day/top.tga");
    dayFaces.push_back("textures/skybox-day/bottom.tga");
    dayFaces.push_back("textures/skybox-day/back.tga");
    dayFaces.push_back("textures/skybox-day/front.tga");

    skyBoxDay.Load(dayFaces);

    std::vector<const GLchar*> nightFaces;
    nightFaces.push_back("textures/skybox-night/right.png");
    nightFaces.push_back("textures/skybox-night/left.png");
    nightFaces.push_back("textures/skybox-night/top.png");
    nightFaces.push_back("textures/skybox-night/bottom.png");
    nightFaces.push_back("textures/skybox-night/back.png");
    nightFaces.push_back("textures/skybox-night/front.png");

    skyBoxNight.Load(nightFaces);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render the scene

    // draw base scene
    for(const std::shared_ptr<Model>& model : models ) {
        model->Draw(myBasicShader);
    }

    // draw sun
    sun.Draw(simpleShader);

    // draw skybox
    if(isDay)
        skyBoxDay.Draw(skyboxShader, view, projection);
    else
        skyBoxNight.Draw(skyboxShader, view, projection);
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
