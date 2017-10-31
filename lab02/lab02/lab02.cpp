// Include C++ headers
#include <iostream>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>   // glm::to_string

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <map>

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

#define W_WIDTH 1024
#define W_HEIGHT 600
#define TITLE "Lab 02"

// Global variables
GLFWwindow* window;
GLuint shaderProgram;
GLuint MVPLocation;
GLuint triangleVAO, cubeVAO;
GLuint triangleVerticiesVBO, triangleColorsVBO, cubeVerticiesVBO, cubeColorsVBO;

void createContext()
{
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("transformation.vertexshader",
        "simple.fragmentshader");

    // Task: Get a pointer location to model matrix in the vertex shader
    MVPLocation = glGetUniformLocation(shaderProgram, "MVP");

    // triangle VAO
    glGenVertexArrays(1, &triangleVAO);
    glBindVertexArray(triangleVAO);

    // vertex VBO
    static const GLfloat triangleVertices[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };
    glGenBuffers(1, &triangleVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // color VBO
    static const GLfloat triangleColors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    glGenBuffers(1, &triangleColorsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat cubeVertices[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    glGenBuffers(1, &cubeVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // color VBO
    static const GLfloat cubeColors[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };
    glGenBuffers(1, &cubeColorsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void free()
{
    glDeleteBuffers(1, &triangleVerticiesVBO);
    glDeleteBuffers(1, &triangleColorsVBO);
    glDeleteVertexArrays(1, &triangleVAO);
    glDeleteBuffers(1, &cubeVerticiesVBO);
    glDeleteBuffers(1, &cubeColorsVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
}

void drawX(mat4 projection, mat4 view, vec3 where)
{
    //glBindVertexArray(cubeVAO);
    static mat4 cubeScaling = scale(mat4(), vec3(0.5, 5, 1));
    static mat4 cubeRotation = rotate(mat4(), radians(25.0f), vec3(0, 0, 1));
    static mat4 cubeRotation2 = rotate(mat4(), radians(-25.0f), vec3(0, 0, 1));
    static mat4 cubeTranslation = translate(mat4(), vec3(0, 0, 0));

    mat4 xTranslation = translate(mat4(), where);

    // Task: cube MVP
    mat4 cubeModel = xTranslation * cubeTranslation * cubeRotation * cubeScaling;
    mat4 cubeMVP = projection * view * cubeModel;
    mat4 cubeModel2 = xTranslation * cubeTranslation * cubeRotation2 * cubeScaling;
    mat4 cubeMVP2 = projection * view * cubeModel2;
    glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cubeMVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cubeMVP2[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void drawV(mat4 projection, mat4 view, vec3 where)
{
    //glBindVertexArray(cubeVAO);
    static mat4 cubeScaling = scale(mat4(), vec3(0.5f, 5.0f, 1.0f));
    static mat4 cubeRotation = rotate(mat4(), radians(20.0f), vec3(0, 0, 1));
    static mat4 cubeRotation2 = rotate(mat4(), radians(-20.0f), vec3(0, 0, 1));
    static mat4 cubeTranslation = translate(mat4(), vec3(-1.9f, 0.0f, 0.0f));
    static mat4 cubeTranslation2 = translate(mat4(), vec3(1.9f, 0.0f, 0.0f));

    mat4 xTranslation = translate(mat4(), where);

    // Task: cube MVP
    mat4 cubeModel = xTranslation * cubeTranslation * cubeRotation * cubeScaling;
    mat4 cubeMVP = projection * view * cubeModel;
    mat4 cubeModel2 = xTranslation * cubeTranslation2 * cubeRotation2 * cubeScaling;
    mat4 cubeMVP2 = projection * view * cubeModel2;
    glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cubeMVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cubeMVP2[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void drawI(mat4 projection, mat4 view, vec3 where)
{
    //glBindVertexArray(cubeVAO);
    static mat4 cubeScaling = scale(mat4(), vec3(0.5f, 5.0f, 1.0f));

    mat4 xTranslation = translate(mat4(), where);

    // Task: cube MVP
    mat4 cubeModel = xTranslation * cubeScaling;
    mat4 cubeMVP = projection * view * cubeModel;
    glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cubeMVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}


void mainLoop()
{
	// Task: transformation
	GLfloat matrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f, // first column
		0.0f, 1.0f, 0.0f, 0.0f, // second column
		0.0f, 0.0f, 1.0f, 0.0f, // third column
		0.0f, 0.0f, 0.0f, 1.0f	// fourth column
	};

	// Task: triangle scale
	mat4 triangleScaling = glm::scale(mat4(), vec3(1, 1, 1));
	//cout << glm::to_string(triangleScaling) << endl;

	// Task: triangle rotate
	mat4 triangleRotation = glm::rotate(mat4(), 3.14f / 4.0f, vec3(0, 0, 1));
	//cout << glm::to_string(triangleRotation) << endl;

	// Task: triangle translate
	mat4 triangleTranslation = glm::translate(mat4(), vec3(0, 0, 0));
	//cout << glm::to_string(triangleTranslation) << endl;

	// Task: cube model matrix
	mat4 cubeScaling = scale(mat4(), vec3(0.5, 5, 1));
	//cout << glm::to_string(cubeScaling) << endl;
	mat4 cubeRotation = rotate(mat4(), radians(45.0f), vec3(0, 0, 1));
	//cout << glm::to_string(cubeRotation) << endl;
	mat4 cubeTranslation = translate(mat4(), vec3(0, 0, 0));
    mat4 cubeTranslation2 = translate(mat4(), vec3(10, 0, 0));
	//cout << glm::to_string(cubeTranslation) << endl;

	// Task: projection
	// Projection matrix: 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	mat4 projection = perspective(
		radians(45.0f),
		4.0f / 3.0f,
		0.1f,
		300.0f
	);
	// Or, for an ortho camera: -x:x, -y:y, -z:z
	// mat4 Projection = ortho(

	// I'm looking from this possition
	vec3 cameraPosition = vec3(0, 0, 250);
	// To the center of coordinates
	vec3 cameraTarget = vec3(0, 0, 0);
	// And the up vector defines "my" view rotation
	vec3 upVector = vec3(0, 1, 0);

	// Task: view
	// Camera view matrix
	mat4 view = lookAt(
		cameraPosition,// the position of your camera, in world space
		cameraTarget,// where you want to look at, in world space
		upVector // probably glm ::vec3(0,1,0), but (0, -1,0) would make you looking upside
	);

    do
    {
        // Task: depth test  | GL_DEPTH_BUFFER_BIT
        // Clear the screen (color and depth)
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // draw triangle
        //glBindVertexArray(triangleVAO);

        // Task: triangle MVP
        // compute model, view projection operator
		//mat4 triangleModel = triangleTranslation *triangleScaling * triangleRotation;
		//mat4 triangleMVP = projection * view * triangleModel;
        // Propagate mvp matrix to shader
        // (&triangleMVP[0][0] -> first component of the sequential array)
		//glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &triangleMVP[0][0]);
        //glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &triangleModel[0][0]);

        //glDrawArrays(GL_TRIANGLES, 0, 3);

        // Enable depth test
        // glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        // glDepthFunc(GL_LESS);

        // draw cube
        //glBindVertexArray(cubeVAO);

        // Task: cube MVP
		// mat4 cubeModel = cubeRotation * cubeScaling;//cubeTranslation * cubeScaling * cubeRotation;
		// mat4 cubeMVP = projection * view * cubeModel;
		// glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cubeMVP[0][0]);

		// Important !!
        // glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
//
        // cubeModel = cubeScaling * cubeTranslation2; // * cubeRotation;
        // cubeMVP = projection * view * cubeModel;
        // glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cubeMVP[0][0]);
//
        // // Important !!
        // glDrawArrays(GL_TRIANGLES, 0, 12 * 3);


        //drawX(projection, view, vec3(20.0f, 0.0f, 0.0f));
        // drawV(projection, view, vec3(0.0f, -30.0f, 0.0f));
        // drawI(projection, view, vec3(5.0f, -30.0f, 0.0f));
//
        // drawX(projection, view, vec3(0.0f, 30.0f, 0.0f));
        // drawI(projection, view, vec3(5.0f, 30.0f, 0.0f));
        // drawI(projection, view, vec3(10.0f, 30.0f, 0.0f));
//
        // drawI(projection, view, vec3(30.0f, 0.0f, 0.0f));
        // drawI(projection, view, vec3(35.0f, 0.0f, 0.0f));
        // drawI(projection, view, vec3(40.0f, 0.0f, 0.0f));
//
        // drawI(projection, view, vec3(-30.0f, 0.0f, 0.0f));
        // drawX(projection, view, vec3(-25.0f, 0.0f, 0.0f));

        std::map<int, std::string> numbers {
                {1, "I"},
                {2, "II"},
                {3, "III"},
                {4, "IV"},
                {5, "V"},
                {6, "VI"},
                {7, "VII"},
                {8, "VIII"},
                {9, "IX"},
                {10, "X"},
                {11, "XI"},
                {12, "XII"},
        };

        for (int i = 1; i < 13; ++i) {
            float diameter = 80;
            float x_pos = (float)sin(i*2*M_PI/12)*diameter;
            float y_pos = (float)cos(i*2*M_PI/12)*diameter;
            float offset = 0;
            for (char c : numbers[i]) {
                switch(c) {
                    case 'I':
                        drawI(projection, view, vec3(x_pos + offset, y_pos, 0.0f));
                        break;
                    case 'V':
                        drawV(projection, view, vec3(x_pos + offset, y_pos, 0.0f));
                        break;
                    default:
                        drawX(projection, view, vec3(x_pos + offset, y_pos, 0.0f));
                }
                offset += 5;
            }

        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Clear the screen (color and depth)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    // Gray background color
    glClearColor(0.4f, 0.3f, 0.3f, 0.0f);

    // Task: Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

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