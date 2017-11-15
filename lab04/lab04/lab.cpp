// Include C++ headers
#include <iostream>
#include <string>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/ModelLoader.h>
#include <common/texture.h>

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();
void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Lab 04"

// Global variables
GLFWwindow* window;
Camera* camera;
GLuint shaderProgram;
GLuint MVPLocation, MLocation, planeLocation, detachmentCoeffLocation;
GLuint modelVAO, modelVerticiesVBO, planeVAO, planeVerticiesVBO;
std::vector<vec3> modelVertices, modelNormals;
std::vector<vec2> modelUVs;

float planeY = 0.0f;
float planeAngle = 0.0f;
float detachmentCoeff = 0.0f;

void createContext()
{
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("Shader.vertexshader", "Shader.fragmentshader");

    // Draw wire frame triangles or fill: GL_LINE, or GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Get a pointer location to model matrix in the vertex shader
    MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
    MLocation = glGetUniformLocation(shaderProgram, "M");

    // Task 3.3d: get uniform location of the plane coefficients
    planeLocation = glGetUniformLocation(shaderProgram, "planeCoeffs");

    // Task 4.1b:
    detachmentCoeffLocation = glGetUniformLocation(shaderProgram, "detachmentDisplacement");

    // model
    loadOBJWithTiny("heart.obj", modelVertices, modelUVs, modelNormals);
    glGenVertexArrays(1, &modelVAO);
    glBindVertexArray(modelVAO);
    glGenBuffers(1, &modelVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(glm::vec3),
        &modelVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // Task 1.1: construct a plane (x-z)
    //*/
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);
    glGenBuffers(1, &planeVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVerticiesVBO);
    float size = 2.0f;
    const GLfloat planeVertices[] = {
        -size, -planeY, -size,
        -size, -planeY,  size,
         size, -planeY,  size,
        // second triangle
        -size, -planeY, -size,
         size, -planeY, -size,
         size, -planeY,  size,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices),
        &planeVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    //*/
}
void free()
{
    glDeleteBuffers(1, &modelVerticiesVBO);
    glDeleteVertexArrays(1, &modelVAO);

    glDeleteBuffers(1, &planeVerticiesVBO);
    glDeleteVertexArrays(1, &planeVAO);

    glDeleteProgram(shaderProgram);
    glfwTerminate();
}

void mainLoop()
{
    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        //camera
        camera->update();
        mat4 projectionMatrix = camera->projectionMatrix;
        mat4 viewMatrix = camera->viewMatrix;

        // Task 1.2: render the plane
        /*/
        glBindVertexArray(planeVAO);
        mat4 planeModelMatrix = mat4(1);
        mat4 planeMVP = projectionMatrix * viewMatrix * planeModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &planeMVP[0][0]);
        glUniformMatrix4fv(MLocation, 1, GL_FALSE, &planeModelMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //*/

        // Task 2.1: translate the plane in + -y direction using the keyboard
        // Task 2.2: and rotate the plane around z-direction
        //*/
        glBindVertexArray(planeVAO);

        // Task 2.1: translation
        vec3 planePosition(0, planeY, 0);
        mat4 planeTranslation = translate(mat4(1), planePosition);

        // Task 2.2: rotation
        mat4 planeRotation = rotate(mat4(1), planeAngle, vec3(0,0,1));

        //mat4 planeModelMatrix = ...;
        //mat4 planeMVP = ...;
        mat4 planeModelMatrix = mat4(1) * planeTranslation * planeRotation;
        mat4 planeMVP = projectionMatrix * viewMatrix * planeModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &planeMVP[0][0]);
        glUniformMatrix4fv(MLocation, 1, GL_FALSE, &planeModelMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //*/

        // Task 3.1: calculate plane coefficients
        //*/
        vec3 planeNormal(planeRotation * vec4(0, 1, 0, 1));
        // vec4 point_x0 = planeTranslation * vec4(0,0,0,1);
        float d = -dot(planeNormal, planePosition);//-planeNormal[0]*point_x0[0] -planeNormal[1]*point_x0[1] -planeNormal[2]*point_x0[2];
        vec4 planeCoeffs(planeNormal, d);
        //*/

        // Task 3.3d:
        //*/
        glUniform4f(planeLocation, planeCoeffs.x, planeCoeffs.y, planeCoeffs.z, planeCoeffs.w);
        //*/

        // Task 4.1b: calculate and transmit the detachment offset to the GPU
        //*/
        vec3 detachmentVec = detachmentCoeff * planeNormal;
        glUniform3f(detachmentCoeffLocation, detachmentVec.x, detachmentVec.y, detachmentVec.z);
        //*/

        // model
        glBindVertexArray(modelVAO);
        mat4 modelModelMatrix = mat4(1);
        mat4 modelMVP = projectionMatrix * viewMatrix * modelModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &modelMVP[0][0]);
        glUniformMatrix4fv(MLocation, 1, GL_FALSE, &modelModelMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, modelVertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
}

void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Task 2.1:
    if (key == GLFW_KEY_I) {
        planeY += 0.01;
    }
    if (key == GLFW_KEY_K) {
        planeY -= 0.01;
    }

    // Task 2.2: planeAngle J, L keys
    if (key == GLFW_KEY_J) {
        planeAngle += 0.01;
    }
    if (key == GLFW_KEY_L) {
        planeAngle -= 0.01;
    }

    // Task 3.4: toggle polygon mode
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);

        if (polygonMode[0] == GL_LINE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        // if GL_LINE, if GL_FILL
    }

    // Task 4.1a: change the detachment coefficient using U, O keys
    if (key == GLFW_KEY_U) {
        detachmentCoeff += 0.01;
    }
    if (key == GLFW_KEY_O) {
        detachmentCoeff -= 0.01;
    }
}

void initialize()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
            " If you have an Intel GPU, they are not 3.3 compatible." +
            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    glfwSetKeyCallback(window, pollKeyboard);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Task 3.3: blend must be enabled
    //*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //*/

    // Log
    logGLParameters();

    // Create camera
    camera = new Camera(window);
}

int main(void)
{
    try
    {
        initialize();
        createContext();
        mainLoop();
        free();
    }
    catch (exception& ex)
    {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }

    return 0;
}