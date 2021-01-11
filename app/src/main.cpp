#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include "Model.hpp"
#include "ColoredModel.hpp"
#include "view_mode.hpp"
#include "Sun.hpp"
#include "lights.hpp"
#include "shadow.hpp"
#include "fog.hpp"
#include "CameraAnimation.hpp"
#include "BabyYoda.hpp"

#include <iostream>

// window
gps::Window myWindow;

// shadows
const unsigned int SHADOW_WIDTH = 4 * 2048;
const unsigned int SHADOW_HEIGHT = 4 * 2048;
GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap = false;

// matrices
glm::mat4 view;
glm::mat4 projection;

// Directional light = Sun
Sun sun;
glm::vec3 sunRotateAxis(1.0f, 1.0f, 1.0f);
const GLfloat sunRotationRadius = 400.0f;
GLfloat sunRotationAngle = 180.0f;
GLfloat sunScaleRadius = 30.0f;
const GLfloat sunScaleMin = 1.0f;
const GLfloat sunScaleMax = 100.0f;
const GLfloat sunScaleDelta = 1.0f;
DirLight sunLight = {
        .direction = glm::vec3(), // calculated based on the position of the sun
        .ambient =  glm::vec3(1.0f, 1.0f, 1.0f), //white light
        .diffuse =  glm::vec3(1.0f, 1.0f, 1.0f), //white light
        .specular =  glm::vec3(1.0f, 1.0f, 1.0f), //white light
};
// The directional light at night produces only the ambient component.
DirLight nightLight = {
        .direction = glm::vec3(), // doesn't matter
        .ambient =  glm::vec3(1.0f, 1.0f, 1.0f), //white light
        .diffuse =  glm::vec3(0.0f, 0.0f, 0.0f), //black light
        .specular =  glm::vec3(0.0f, 0.0f, 0.0f), //black light
};

// Day or Night
bool isDay = true;

// Point lights = fireflies
std::vector<PointLight> pointLights{
        {
                .position = glm::vec3(19.488f - 2.3786f, 1.5f, 10.514f - 5.0f),
                .constant = 1.0f,
                .linear = 0.35f,
                .quadratic = 0.44f,
                .ambient = glm::vec3(1.0f, 1.0f, 0.0f), //yellow light
                .diffuse = glm::vec3(1.0f, 1.0f, 0.0f), //yellow light
                .specular = glm::vec3(1.0f, 1.0f, 0.0f), //yellow light
        },
        {
                .position = glm::vec3(-16.179f + 0.5f, 2.0f, 20.967f - 2.0f),
                .constant = 1.0f,
                .linear = 0.35f,
                .quadratic = 0.44f,
                .ambient = glm::vec3(1.0f, 1.0f, 0.2f), //yellowish light
                .diffuse = glm::vec3(1.0f, 1.0f, 0.2f), //yellowish light
                .specular = glm::vec3(1.0f, 1.0f, 0.2f), //yellowish light
        }
};
std::vector<ColoredModel> fireflies;

// Fog = Sandstorm
Fog fog{
        .position = glm::vec3(0.0f, 0.0f, 0.0f),
        .color = glm::vec4(0.76f, 0.69f, 0.5f, 1.0f), // sand color
        .density = 0.00f
};
const float fogMin = 0.0f;
const float fogMax = 0.05f;
const float fogDelta = 0.001f;

// camera
const GLfloat cameraSpeed = 0.5f;
const GLfloat cameraHeight = 2.0f;
const GLfloat cameraCollisionRadius = 1.0f;
const GLfloat cameraCollisionBottomHeight = 0.5f;
gps::Camera myCamera(
        glm::vec3(0.0f, cameraHeight, 0.0f),
        glm::vec3(0.0f, cameraHeight, -10.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

// camera animation
std::vector<glm::vec3> cameraControlPoints{
        glm::vec3(-3.93687f, 2.0f, 10.6028f), // halfway between baby yoda and center
        glm::vec3(0.0f, 2.0f, 0.0f), // center of scene - building
        glm::vec3(-11.6519f, 2.0f, -5.9787f), // starship
        glm::vec3(-9.06584f, 2.0f, -15.9509f), // building closeup
        glm::vec3(8.31961f, 2.0f, -12.024f), // vaporizator
        glm::vec3(11.9526f, 2.0f, 6.16476f), // fruit stand
        glm::vec3(15.3243f, 1.0f, 7.35698f), // fruit stand closeup
        glm::vec3(12.5138f, 2.0f, 14.4142f), // seats
        glm::vec3(15.0508f, 1.0f, 16.2713), // seats closeup
        glm::vec3(10.7486f, 2.0f, 23.1867f), // R2-D2
        glm::vec3(-7.87374f, 2.0f, 21.2056f), // baby Yoda (Grogu)
};
GLfloat cameraSpeedAnimated = 0.2f;
bool isCameraAnimated = false;
CameraAnimation cameraAnimation(cameraSpeedAnimated, CameraAnimation::CENTRIPETAL, cameraControlPoints, myCamera);

ViewMode viewMode = ViewMode::SOLID_SMOOTH;


GLboolean pressedKeys[1024];

// models
Model baseScene;
BabyYoda babyYoda;
float babyYodaAnimationSpeed = 1.0f;
Model ship;
gps::Model3D screenQuad;
std::vector<Model *> models;
// models for which collision detection is enabled
std::vector<Model *> modelsColliding;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader simpleShader;
gps::Shader simpleColorShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
// Shaders that require view and projection matrices
std::vector<gps::Shader *> shaders;
// Shaders that require data of lights (directional or positional)
std::vector<gps::Shader *> shadersLights;
// Shaders that require the light space transformation matrix
std::vector<gps::Shader *> shadersLightSpTrMat;
// Shaders that require data of the fog
std::vector<gps::Shader *> shadersFog;

// skybox
gps::SkyBox skyBoxDay;
gps::SkyBox skyBoxNight;

// constants
float zNear = 0.1f;
float zFar = 500.0f;
bool collisionEnabled = true;

GLenum glCheckError_(const char *file, int line) {
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
    for (const auto &shader : shaders) {
        shader->useShaderProgram();
        shader->setUniform("view", view);
    }

    if (updateNormals) {
        // update the normal matrices of the models
        for (const auto &model : models) {
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
    for (const auto &shader : shaders) {
        shader->useShaderProgram();
        shader->setUniform("projection", projection);
    }
}

void updateSunlight() {
    static const glm::vec3 axisY = glm::vec3(0.0f, 1.0f, 0.0f);

    // The direction of the directional light is from the fragment towards the light source
    // The light direction is the vector from the origin to the center of the sun object.
    // lightDir = sunPosition - (0, 0, 0)
    sunLight.direction = glm::normalize(sun.getPosition());
    nightLight.direction = glm::normalize(sun.getPosition());

    isDay = angleBetween(axisY, sunLight.direction) < 80.0f;

    // send the directional sunlight data to the shaders which depend on it
    for (const auto &shader : shadersLights) {
        shader->useShaderProgram();
        // the sun has a unit radius. By scaling it, the radius will proportionally change.
        // turn of sunlight if the sun is fully beneath the ground
        if (sun.getPosition().y + sunScaleRadius > 0) {
            sendDirLight(sunLight, *shader);
        } else {
            sendDirLight(nightLight, *shader);
        }
    }

    const glm::mat4 &lightSpaceTrMat = computeLightSpaceTrMatrix(sunLight.direction, baseScene.getBoundingBox());

    // update the light space transformation matrices in the shaders which depend on it
    for (const auto &shader : shadersLightSpTrMat) {
        shader->useShaderProgram();
        shader->setUniform("lightSpaceTrMatrix", lightSpaceTrMat);
    }
}

void rotateSun(float angle) {
    sun.rotate(angle);
    updateSunlight();
}

void scaleSun(float delta) {
    sunScaleRadius = glm::clamp(sunScaleRadius + delta, sunScaleMin, sunScaleMax);
    sun.setScale(sunScaleRadius);
}

void updateFog() {
    //send fog to shaders
    for (const auto &shader : shadersFog) {
        shader->useShaderProgram();
        sendFog(fog, *shader);
    }
}

void increaseFogDensity(float delta) {
    fog.density = glm::clamp(fog.density + delta, fogMin, fogMax);
    updateFog();
}

bool collidesWithModels(const BoundingBox &boundingBox) {
    for (const auto &model : modelsColliding) {
        if (model->collidesWith(boundingBox))
            return true;
    }
    return false;
}

void moveCamera(gps::MOVE_DIRECTION direction) {
    if (!isCameraAnimated) {
        if (collisionEnabled) {
            const glm::vec3 &cameraPosition = myCamera.nextPosition(direction, cameraSpeed);

            BoundingBox cameraBoundingBox{
                    .min = glm::vec4(
                            cameraPosition.x - cameraCollisionRadius,
                            cameraCollisionBottomHeight,
                            cameraPosition.z - cameraCollisionRadius,
                            1.0f
                    ),
                    .max = glm::vec4(cameraPosition + cameraCollisionRadius, 1.0f)
            };

            if (collidesWithModels(cameraBoundingBox))
                return;
        }

        myCamera.move(direction, cameraSpeed);
        updateViewMatrix();
    }
}

void windowResizeCallback(GLFWwindow *window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);

    myWindow.setWindowDimensions({width, height});

    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    updateProjectionMatrix();
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
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

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (isCameraAnimated)
        return;

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

void processKeys() {
    if (pressedKeys[GLFW_KEY_Q]) {
        if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
            scaleSun(-sunScaleDelta);
        } else if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
            increaseFogDensity(-fogDelta);
        } else {
            rotateSun(1.0f);
        }
    }

    if (pressedKeys[GLFW_KEY_E]) {
        if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
            scaleSun(sunScaleDelta);
        } else if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
            increaseFogDensity(fogDelta);
        } else {
            rotateSun(-1.0f);
        }
    }

    if (pressedKeys[GLFW_KEY_M]) {
        showDepthMap = !showDepthMap;
        pressedKeys[GLFW_KEY_M] = 0;
    }

    if (pressedKeys[GLFW_KEY_V]) {
        nextViewMode(myBasicShader, viewMode);
        pressedKeys[GLFW_KEY_V] = 0;
    }

    if (pressedKeys[GLFW_KEY_C]) {
        pressedKeys[GLFW_KEY_C] = 0;
        if (!isCameraAnimated) {
            cameraAnimation.reset();
        }
        isCameraAnimated = !isCameraAnimated;
    }

    if (pressedKeys[GLFW_KEY_B]) {
        pressedKeys[GLFW_KEY_B] = 0;
        collisionEnabled = !collisionEnabled;
    }

    // Process camera movements
    // At the end, in order to do all model transformations before it.

    if (pressedKeys[GLFW_KEY_W]) {
        moveCamera(gps::MOVE_FORWARD);
    }

    if (pressedKeys[GLFW_KEY_S]) {
        moveCamera(gps::MOVE_BACKWARD);
    }

    if (pressedKeys[GLFW_KEY_A]) {
        moveCamera(gps::MOVE_LEFT);
    }

    if (pressedKeys[GLFW_KEY_D]) {
        moveCamera(gps::MOVE_RIGHT);
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
    glClearColor(fog.color.r, fog.color.g, fog.color.b, fog.color.a);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    // Enable mouse control
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initShaders() {
    myBasicShader.loadShader(
            "shaders/basic.vert",
            "shaders/basic.frag",
            "shaders/basic.geom");
    simpleShader.loadShader("shaders/simpleTex.vert",
                            "shaders/simpleTex.frag");
    simpleColorShader.loadShader("shaders/simple.vert",
                                 "shaders/uniformColor.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert",
                            "shaders/skyboxShader.frag");
    screenQuadShader.loadShader("shaders/screenQuad.vert",
                                "shaders/screenQuad.frag");
    depthMapShader.loadShader("shaders/depthMapShader.vert",
                              "shaders/passThrough.frag");

    // Add shaders to the list of shaders that require view and projection matrices
    shaders.push_back(&myBasicShader);
    shaders.push_back(&simpleShader);
    shaders.push_back(&simpleColorShader);

    // Add shaders to the list of shaders that require data of lights (directional or positional)
    shadersLights.push_back(&myBasicShader);

    // Add shaders to the list of shaders that require the light space transformation matrix
    shadersLightSpTrMat.push_back(&myBasicShader);
    shadersLightSpTrMat.push_back(&depthMapShader);

    // Add shaders to the list of shaders that require data of the fog
    shadersFog.push_back(&myBasicShader);
}

void initLights() {
    sun.LoadModel("models/sun/sun.obj");
    sun.init(sunRotateAxis, sunRotationRadius, sunScaleRadius, sunRotationAngle);

    // Create firefly model for each point light
    for (const auto &pointLight : pointLights) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLight.position);

        fireflies.emplace_back();
        ColoredModel &firefly = fireflies.back();
        firefly.LoadModel("models/firefly/firefly.obj");
        firefly.init(model);
        firefly.color = pointLight.diffuse;
    }

    // send the positional light data to the shaders which depend on it
    for (const auto &shader : shadersLights) {
        shader->useShaderProgram();
        sendPointLights(pointLights, *shader);
    }
}

void initModels() {
    baseScene.LoadModel("models/scene/scene.obj");
    baseScene.init(glm::mat4(1.0f), view);
    babyYoda.LoadModel("models/baby_yoda/baby_yoda.obj");
    babyYoda.init(glm::mat4(1.0f), view);
    ship.LoadModel("models/ship/ship.obj");
    ship.init(glm::mat4(1.0f), view);
    screenQuad.LoadModel("models/quad/quad.obj");

    models.push_back(&baseScene);
    models.push_back(&babyYoda);
    models.push_back(&ship);

    modelsColliding.push_back(&baseScene);
    modelsColliding.push_back(&babyYoda);
    modelsColliding.push_back(&ship);
}

void initSkyBox() {
    std::vector<const GLchar *> dayFaces;
    dayFaces.push_back("textures/skybox-day/right.tga");
    dayFaces.push_back("textures/skybox-day/left.tga");
    dayFaces.push_back("textures/skybox-day/top.tga");
    dayFaces.push_back("textures/skybox-day/bottom.tga");
    dayFaces.push_back("textures/skybox-day/back.tga");
    dayFaces.push_back("textures/skybox-day/front.tga");

    skyBoxDay.Load(dayFaces);

    std::vector<const GLchar *> nightFaces;
    nightFaces.push_back("textures/skybox-night/right.png");
    nightFaces.push_back("textures/skybox-night/left.png");
    nightFaces.push_back("textures/skybox-night/top.png");
    nightFaces.push_back("textures/skybox-night/bottom.png");
    nightFaces.push_back("textures/skybox-night/back.png");
    nightFaces.push_back("textures/skybox-night/front.png");

    skyBoxNight.Load(nightFaces);
}

void initUniforms() {
    // Update the view matrix of the dependent shaders
    // Also updates the normal matrices of the models
    updateViewMatrix();

    // Update the projection matrix of the dependent shaders
    updateProjectionMatrix();

    // update the directional light
    updateSunlight();

    // Set the view mode
    setViewMode(myBasicShader, viewMode);

    // Send fog data to shaders
    updateFog();
}

void initAnimations() {
    babyYoda.startAnimation(babyYodaAnimationSpeed);
}

void drawObjects(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    for (const auto &model : models) {
        shader.setUniform("model", model->getModelMatrix());
        // do not send the normal matrix if we are rendering in the depth map
        if (!depthPass)
            shader.setUniform("normalMatrix", model->getNormalMatrix());
        model->Draw(shader);
    }
}

void renderScene() {
    // animate camera
    if (isCameraAnimated) {
        cameraAnimation.animate();
        updateViewMatrix();
    }

    // depth maps creation pass
    // Render the scene in the depth map
    // Render the scene to the depth buffer

    depthMapShader.useShaderProgram();

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    drawObjects(depthMapShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // configure back the viewport
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    if (showDepthMap) {
        // render depth map on screen

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        screenQuadShader.setUniform("depthMap", 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    } else {
        // final scene rendering pass (with shadows)

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //bind the shadow map
        myBasicShader.useShaderProgram();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        myBasicShader.setUniform("shadowMap", 3);

        //render the scene
        drawObjects(myBasicShader, false);

        // draw sun
        sun.Draw(simpleShader);

        // draw fireflies
        simpleColorShader.useShaderProgram();
        for (auto &firefly : fireflies) {
            simpleColorShader.setUniform("model", firefly.getModelMatrix());
            simpleColorShader.setUniform("color", glm::vec4(firefly.color, 1.0f));
            firefly.Draw(simpleColorShader);
        }

        // draw skybox
        if (isDay)
            skyBoxDay.Draw(skyboxShader, view, projection);
        else
            skyBoxNight.Draw(skyboxShader, view, projection);
    }
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char *argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initShaders();
    initLights();
    initModels();
    initSkyBox();
    initUniforms();
    initAnimations();
    initShadowMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT, shadowMapFBO, depthMapTexture);
    setWindowCallbacks();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        renderScene();
        processKeys();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
