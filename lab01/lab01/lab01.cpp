// Include C++ headers
#include <iostream>
#include <string>

// Include GLEW (always include first)
#include <GL/glew.h>

// Include GLFW to handle window and keyboard events
#include <glfw3.h>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>

using namespace std;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Lab 01"

// Global variables
GLFWwindow* window;
GLuint shaderProgram;
GLuint triangleVAO;
GLuint verticesVBO, colorsVBO;

void createContext()
{
    // Create and compile our GLSL program from the shaders
	shaderProgram = loadShaders("simple.vertexshader", "simple.fragmentshader");

	/* geometry to use. these are 3 xyz points (9 floats total) to make a triangle */
	const GLfloat vertices[] = {
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

    // task define VAO

	/* The vertex array object (VAO) is a little descriptor that defines which
	data from vertex buffer objects should be used as input variables to vertex
	shaders. In our case - use our only VBO, and say 'every three floats is a
	variable' */
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

    // task define vertex VBO
	/* a vertex buffer object (VBO) is created here. this stores an array of
	data on the graphics adapter's memory. in our case - the vertex points */
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	// "attribute #0 is created from every 3 variables in the above buffer, of type
	// float (i.e. make me vec3s)"
	glVertexAttribPointer(
		0, // attribute 0 (must match the layout in the shader)
		3, // size: xyz
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		NULL // array buffer offset
	);

    // Draw wire frame triangles or fill: GL_LINE, or GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // enable point size
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void free()
{
    // Free allocated buffers
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &colorsVBO);
    glDeleteVertexArrays(1, &triangleVAO);
    glDeleteProgram(shaderProgram);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}

void mainLoop()
{
    do
    {
        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw

        // task bind shader
		glUseProgram(shaderProgram);

        // task bind VAO
		glBindVertexArray(triangleVAO);

		//"attribute #0 and #1 should be enabled when this VAO is bound"
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		/* draw points 0-3 as triangles from the currently bound VAO with
		current in-use shader*/
		glDrawArrays(GL_TRIANGLES, 0, 3); // interpret as triangles read 3 data

        // task draw triangle

        // task draw points

        // Swap buffers
        glfwSwapBuffers(window);

        // Events
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
}

void initialize()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);  // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

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

    // Set background color (gray) [r, g, b, a]
    glClearColor(0.75f, 0.75f, 0.75f, 0.0f);

    // Log
    logGLParameters();
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