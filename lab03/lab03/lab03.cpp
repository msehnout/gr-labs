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

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Lab 03"

// Global variables
GLFWwindow* window;
Camera* camera;
GLuint shaderProgram;
GLuint MVPLocation;
GLuint textureSampler;
GLuint texture;
GLuint suzanneVAO;
GLuint suzanneVerticiesVBO, suzanneUVVBO;
std::vector<vec3> suzanneVertices, suzanneNormals;
std::vector<vec2> suzanneUVs;

void createContext()
{
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("texture.vertexshader", "texture.fragmentshader");

    // Draw wire frame triangles or fill: GL_LINE, or GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Get a pointer location to model matrix in the vertex shader
    MVPLocation = glGetUniformLocation(shaderProgram, "MVP");

    // Task 1: load
    loadOBJ("suzanne.obj", suzanneVertices, suzanneUVs, suzanneNormals);

    // VAO
    glGenVertexArrays(1, &suzanneVAO);
    glBindVertexArray(suzanneVAO);

    // vertex VBO
    glGenBuffers(1, &suzanneVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, suzanneVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, suzanneVertices.size() * sizeof(glm::vec3),
        &suzanneVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // Task 6: texture loading
    // uncomment Task 6 in main loop
    //*/
    // Get a handle for our "textureSampler" uniform
    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");

    // load BMP
    texture = loadBMP("uvtemplate.bmp");

    // uvs VBO
    glGenBuffers(1, &suzanneUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, suzanneUVVBO);
    glBufferData(GL_ARRAY_BUFFER, suzanneUVs.size() * sizeof(glm::vec2),
        &suzanneUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //*/
}
void free()
{
    glDeleteBuffers(1, &suzanneVerticiesVBO);
    glDeleteBuffers(1, &suzanneUVVBO);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &suzanneVAO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}

void mainLoop()
{
	unsigned int t = 0;
    do
    {
        mat4 MVP, modelMatrix, viewMatrix, projectionMatrix;

        // Task 2: perspective projection
        
        // Projection matrix: 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        projectionMatrix = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        // Task 2.1: change z translation and define MVP
        modelMatrix = translate(mat4(), vec3(0.0, 0.0, -10));
        MVP = projectionMatrix * modelMatrix;
        //

        // Task 3: ortho projection
        /*/ <== TADY ODKOMENTOVAT PRO ROTUJICI OBRAZ
        // Task 3.1: change z
        projectionMatrix = ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 10.0f); // In world coordinates
        // Task 3.2: change z translation
        modelMatrix = translate(mat4(), vec3(0.0, 0.0, -10.5));
        MVP = projectionMatrix * modelMatrix;
        //*/

        // Task 4: view
        /*/
        // Task 4.1: make the camera move linearly in time
        // Task 4.2: make the camera move periodically around the object
        viewMatrix = lookAt(
            vec3(10 * cos((float)t / 500.0), sin(t/100.0), 10*sin((float)t/500.0)), // Camera position, in World Space
            vec3(0, 0, 0), // and looks at the origin
            vec3(0, 1, 0)  // Head is up (set to 0, -1, 0 to look upside-down)
        );
		++t;
        modelMatrix = mat4(1.0);

        MVP = projectionMatrix * viewMatrix * modelMatrix;
        //*/

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // suzanne
        glBindVertexArray(suzanneVAO);

        // Task 5: camera
        //*/
        camera->update();
        projectionMatrix = camera->projectionMatrix;
        viewMatrix = camera->viewMatrix;
        modelMatrix = glm::mat4(1.0);
        MVP = projectionMatrix * viewMatrix * modelMatrix;
        //*/

        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);

        // Task 6: texture
        /*/
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Set our "textureSampler" sampler to use Texture Unit 0
        glUniform1i(textureSampler, 0);
        //*/

        // draw
        glDrawArrays(GL_TRIANGLES, 0, suzanneVertices.size());

        glfwSwapBuffers(window);

        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
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

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Task 6.1
    // Enable blending for transparency
    // change alpha in fragment shader
    //*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //*/

    // Task 6.2
    // Cull triangles which normal is not towards the camera
    //*/
    glEnable(GL_CULL_FACE);
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