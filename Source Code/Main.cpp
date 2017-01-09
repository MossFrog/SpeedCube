#include <iostream>
#include <vector>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL\SOIL.h>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

#include "Shader.h"

using namespace std;

//-- Global Defínitions --//
const GLint WIDTH = 1000, HEIGHT = 600;
glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


glm::vec3 freecameraPos = glm::vec3(0.0f, 5.0f, 3.0f);
glm::vec3 freecameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 freecameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 playerPos = glm::vec3(5.0f, 0.0f, 0.0f);
glm::vec3 destPos = glm::vec3(5.0f, 0.0f, 0.0f);

//-- Object Hovering --//
GLfloat objectYpos = 0.0f;


//-- The main key callback method that allows GLFW to receive and handle user input --//
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
//-- The main mouse callback method for GLFW mouse activity controls --//
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//-- Distance returning function between two glm::vec3 points --//
double distanceCalc(glm::vec3 pointOne, glm::vec3 pointTwo);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

GLfloat forwardRotationAngle = 0.0f;
GLfloat sidewardRotationAngle = 0.0f;

GLfloat cameraSpeed = 0.10f;

int animCounter = 0; //-- Counter for the player movement animation loops --//
int animDirection = 0; //-- Animation directions are 0 - North 1 - South 2 - East and 3 - West --//

bool animEnabled = true;

//-- Camera Controls --//

bool freeformEnabled = false;
//-- Last mouse position Calculation --//
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;


//-- We utilize SFML's clock methods for rendering since using GLFLoats slows down the system over time --//
//-- accumulating random crap in the VRAM --//
sf::Clock mainClock;
sf::Clock playerAnimationClock;

sf::Clock freeFormClock;


//-- The player moves in 3.1 unit incremental steps on the X-Z plane --//
//-- Thus any collisible and collectible objects must be within the player's path --//

int main()
{
	//-- Initial Definitions Section --//
	mainClock.restart();
	playerAnimationClock.restart();
	freeFormClock.restart();

	//--------------------------------//

	//- Initialize GLFW -//
	glfwInit();

	//-- Necessary Setups for GLFW --//
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//-- We allocate and create a pointer to the GLFW Window object --//
	GLFWwindow * mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Speed Cube", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(mainWindow, &screenWidth, &screenHeight);

	if (nullptr == mainWindow)
	{
		cout << "GLFW Window failed to initialize!" << std::endl;
		glfwTerminate();
		return 1;
	}

	//-- Set the main window to the current context --//
	glfwMakeContextCurrent(mainWindow);

	//-- This set operation indicates that we are utilizing the modern shorthands for GLEW --//
	glewExperimental = GL_TRUE;


	//-- Initialize GLEW to setup the OpenGL Function pointers --//
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return 1;
	}

	//-- Viewport Dimension Setting --//
	glViewport(0, 0, screenWidth, screenHeight);

	//-- Designate the keyCallback Method --//
	glfwSetKeyCallback(mainWindow, keyCallback);

	//-- Designate the mouseCallback Method --//
	glfwSetCursorPosCallback(mainWindow, mouse_callback);

	//-- Enable Depth Testing --//
	glEnable(GL_DEPTH_TEST);

	//-- GL Settings --//
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//-- Building and Compiling the Shader Prorgams --//
	Shader ourShader("./Shaders/vertex.vert", "./Shaders/fragment.frag");
	Shader shaderTree("./Shaders/TreeVertex.vert", "./Shaders/TreeFragment.frag");
	Shader shaderPlayer("./Shaders/PlayerVertex.vert", "./Shaders/PlayerFragment.frag");
	Shader shaderShadow("./Shaders/ShadowVertex.vert", "./Shaders/ShadowFragment.frag");
	Shader shaderLight("./Shaders/ShadowVertex.vert", "./Shaders/LightFragment.frag");
	//-- Initialization of Variables --//

	//-- Vertex and Buffer Data --//


	//-- Cube Vertex Array Definition --//
	GLfloat PlaneVertices[] = {

		-50.0f, -0.5f, -50.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		50.0f, -0.5f, -50.0f, 50.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		50.0f, -0.5f, 50.0f, 50.0f, 50.0f, 0.0f, 1.0f, 0.0f,
		50.0f, -0.5f, 50.0f, 50.0f, 50.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, -0.5f, 50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, -0.5f, -50.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};

	GLfloat DumbPlaneVertices[] = {

		-1.0f, -0.48f, -1.0f,	0.0f, 0.0f,
		 1.0f, -0.48f, -1.0f, 1.0f, 0.0f,
		1.0f, -0.48f, 1.0f, 1.0f, 1.0f,
		1.0f, -0.48f, 1.0f, 1.0f, 1.0f,
		-1.0f, -0.48f, 1.0f, 0.0f, 1.0f,
		-1.0f, -0.48f, -1.0f, 0.0f, 0.0f
	};

	GLfloat CubeVertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
	};

	GLfloat LightCubeVertices[] = {
		-0.25f, -0.25f, -0.25f, 0.0f, 0.0f,
		0.25f, -0.25f, -0.25f, 1.0f, 0.0f,
		0.25f, 0.25f, -0.25f, 1.0f, 1.0f,
		0.25f, 0.25f, -0.25f, 1.0f, 1.0f,
		-0.25f, 0.25f, -0.25f, 0.0f, 1.0f,
		-0.25f, -0.25f, -0.25f, 0.0f, 0.0f,

		-0.25f, -0.25f, 0.25f, 0.0f, 0.0f,
		0.25f, -0.25f, 0.25f, 1.0f, 0.0f,
		0.25f, 0.25f, 0.25f, 1.0f, 1.0f,
		0.25f, 0.25f, 0.25f, 1.0f, 1.0f,
		-0.25f, 0.25f, 0.25f, 0.0f, 1.0f,
		-0.25f, -0.25f, 0.25f, 0.0f, 0.0f,

		-0.25f, 0.25f, 0.25f, 1.0f, 0.0f,
		-0.25f, 0.25f, -0.25f, 1.0f, 1.0f,
		-0.25f, -0.25f, -0.25f, 0.0f, 1.0f,
		-0.25f, -0.25f, -0.25f, 0.0f, 1.0f,
		-0.25f, -0.25f, 0.25f, 0.0f, 0.0f,
		-0.25f, 0.25f, 0.25f, 1.0f, 0.0f,

		0.25f, 0.25f, 0.25f, 1.0f, 0.0f,
		0.25f, 0.25f, -0.25f, 1.0f, 1.0f,
		0.25f, -0.25f, -0.25f, 0.0f, 1.0f,
		0.25f, -0.25f, -0.25f, 0.0f, 1.0f,
		0.25f, -0.25f, 0.25f, 0.0f, 0.0f,
		0.25f, 0.25f, 0.25f, 1.0f, 0.0f,

		-0.25f, -0.25f, -0.25f, 0.0f, 1.0f,
		0.25f, -0.25f, -0.25f, 1.0f, 1.0f,
		0.25f, -0.25f, 0.25f, 1.0f, 0.0f,
		0.25f, -0.25f, 0.25f, 1.0f, 0.0f,
		-0.25f, -0.25f, 0.25f, 0.0f, 0.0f,
		-0.25f, -0.25f, -0.25f, 0.0f, 1.0f,

		-0.25f, 0.25f, -0.25f, 0.0f, 1.0f,
		0.25f, 0.25f, -0.25f, 1.0f, 1.0f,
		0.25f, 0.25f, 0.25f, 1.0f, 0.0f,
		0.25f, 0.25f, 0.25f, 1.0f, 0.0f,
		-0.25f, 0.25f, 0.25f, 0.0f, 0.0f,
		-0.25f, 0.25f, -0.25f, 0.0f, 1.0f
	};

	GLfloat ColoredCubeVertices[] = {
		-0.5f, -0.5f, -0.5f,	 1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 	0.0f,  0.0f,  -1.0f,
		0.5f, -0.5f, -0.5f,		 0.0f, 0.0f, 1.0f,	1.0f, 0.0f, 	0.0f,  0.0f,  -1.0f,
		0.5f, 0.5f, -0.5f,		 1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 	0.0f,  0.0f,  -1.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 	0.0f,  0.0f,  -1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 	0.0f,  0.0f,  -1.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 	0.0f,  0.0f,  -1.0f,


    	-0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	 0.0f, 0.0f, 	0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, 0.5f,		0.0f, 1.0f, 0.0f,	 1.0f, 0.0f, 	0.0f,  0.0f,  1.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	 1.0f, 1.0f, 	0.0f,  0.0f,  1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	 1.0f, 1.0f, 	0.0f,  0.0f,  1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,	 0.0f, 1.0f, 	0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	 0.0f, 0.0f, 	0.0f,  0.0f,  1.0f,


    	-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,	 1.0f, 0.0f, 	-1.0f,  0.0f,  0.0f,
		-0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	 1.0f, 1.0f, 	-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f, 0.0f,	 0.0f, 1.0f, 	-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f, 0.0f,	 0.0f, 1.0f, 	-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	 0.0f, 0.0f, 	-1.0f,  0.0f,  0.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,	 1.0f, 0.0f, 	-1.0f,  0.0f,  0.0f,

    	0.5f, 0.5f, 0.5f,		1.0f, 0.5f, 0.5f,	 1.0f, 0.0f, 	1.0f,  0.0f,  0.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 0.0f, 1.0f,	 1.0f, 1.0f, 	1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 1.0f,	 0.0f, 1.0f, 	1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.5f, 0.5f,	 0.0f, 1.0f, 	1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	 0.0f, 0.0f, 	1.0f,  0.0f,  0.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	 1.0f, 0.0f, 	1.0f,  0.0f,  0.0f,

    	-0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 1.0f,	 0.0f, 1.0f, 	0.0f,  -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.5f, 0.0f,	 1.0f, 1.0f, 	0.0f,  -1.0f,  0.0f,
		0.5f, -0.5f, 0.5f,		0.0f, 0.5f, 1.0f,	 1.0f, 0.0f, 	0.0f,  -1.0f,  0.0f,
		0.5f, -0.5f, 0.5f,	    1.0f, 0.0f, 1.0f,	 1.0f, 0.0f, 	0.0f,  -1.0f,  0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.5f, 0.0f,	 0.0f, 0.0f, 	0.0f,  -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.5f, 1.0f,	 0.0f, 1.0f, 	0.0f,  -1.0f,  0.0f,

    	-0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 1.0f,	 0.0f, 1.0f, 	0.0f,  1.0f,  0.0f,
		0.5f, 0.5f, -0.5f,	    0.0f, 0.5f, 0.0f,	 1.0f, 1.0f, 	0.0f,  1.0f,  0.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 1.0f,	 1.0f, 0.0f, 	0.0f,  1.0f,  0.0f,
		0.5f, 0.5f, 0.5f,	    1.0f, 0.0f, 1.0f,	 1.0f, 0.0f, 	0.0f,  1.0f,  0.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 0.5f, 0.0f,	 0.0f, 0.0f, 	0.0f,  1.0f,  0.0f,
		-0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 1.0f,	 0.0f, 1.0f, 	0.0f,  1.0f,  0.0f
	};


	//-- Positions of trees --//
	vector<glm::vec3> treePosVect =
	{
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(-1.5f, 0.0f, 4.0f),
		glm::vec3(-3.8f, 0.0f, 3.0f),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(-1.5f, 1.0f, 4.0f),
		glm::vec3(-3.8f, 1.0f, 3.0f),

		glm::vec3(1.0f, 0.0f, -2.0f),
		glm::vec3(1.5f, 0.0f, -6.0f),
		glm::vec3(3.8f, 0.0f, -3.0f)
	};

	//-- Positions of Moving Lights --//

	//-- Clockwise --//
	vector<glm::vec3> CWlightPosVect =
	{
		glm::vec3(42.0f, 1.0f, 1.0f),
		glm::vec3(-33.5f, 1.0f, 22.0f),
		glm::vec3(-6.8f, 1.0f, -18.0f),
		glm::vec3(13.0f, 1.0f, 17.0f),
		glm::vec3(0.5f, 1.0f, 0.0f),
		glm::vec3(-33.5f, 1.0f, 22.0f),
		glm::vec3(-6.8f, 1.0f, -18.0f),
		glm::vec3(13.0f, 1.0f, 17.0f),
		glm::vec3(-16.5f, 1.0f, -18.0f),
		glm::vec3(-33.5f, 1.0f, 22.0f),
		glm::vec3(-6.8f, 1.0f, -18.0f),
		glm::vec3(13.0f, 1.0f, 17.0f),
		glm::vec3(-16.5f, 1.0f, -18.0f),
		glm::vec3(-42.8f, 1.0f, 32.0f)
	};

	//-- Counter Clockwise (Unused) --//
	vector<glm::vec3> CCWlightPosVect =
	{
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(-1.5f, 1.0f, 4.0f),
		glm::vec3(-3.8f, 1.0f, 3.0f),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(-1.5f, 1.0f, 4.0f),
		glm::vec3(-3.8f, 1.0f, 3.0f)
	};

	//-- Player VAO and VBO --//
	GLuint PlayerVBO, PlayerVAO;
	glGenVertexArrays(1, &PlayerVAO);
	glGenBuffers(1, &PlayerVBO);

	glBindVertexArray(PlayerVAO);

	glBindBuffer(GL_ARRAY_BUFFER, PlayerVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ColoredCubeVertices), ColoredCubeVertices, GL_STATIC_DRAW);

	//-- Position Attribute of the Vertices --//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//-- Texture Co-Ordinate Attribute of the Vertices --//
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//-- Normal Vector Attribute of the Vertices --//
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	//-- Tree VAO and VBO --//
	GLuint TreeVBO, TreeVAO;
	glGenVertexArrays(1, &TreeVAO);
	glGenBuffers(1, &TreeVBO);

	glBindVertexArray(TreeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, TreeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

	//-- Position Attribute of the Vertices --//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//-- Texture Co-Ordinate Attribute of the Vertices --//
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	//-- Plane VAO and VBO --//
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PlaneVertices), PlaneVertices, GL_STATIC_DRAW);

	//-- Position Attribute of the Vertices --//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//-- Texture Co-Ordinate Attribute of the Vertices --//
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//-- Normal Vectors --//
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	//-- Shadow VAO and VBO --//
	GLuint ShadowVBO, ShadowVAO;
	glGenVertexArrays(1, &ShadowVAO);
	glGenBuffers(1, &ShadowVBO);

	glBindVertexArray(ShadowVAO);

	glBindBuffer(GL_ARRAY_BUFFER, ShadowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DumbPlaneVertices), DumbPlaneVertices, GL_STATIC_DRAW);

	//-- Position Attribute of the Vertices --//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//-- Texture Co-Ordinate Attribute of the Vertices --//
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);


	//-- Light Cubes VAO and VBO --//
	GLuint LightCubeVBO, LightCubeVAO;
	glGenVertexArrays(1, &LightCubeVAO);
	glGenBuffers(1, &LightCubeVBO);

	glBindVertexArray(LightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, LightCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LightCubeVertices), LightCubeVertices, GL_STATIC_DRAW);

	//-- Position Attribute of the Vertices --//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//-- Texture Co-Ordinate Attribute of the Vertices --//
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	

	// ====================
	// Texture 1 (Metal Plates)
	// ====================
	GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("./metalWall2.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.



	// ====================
	// Texture 2 ('Leaves')
	// ====================
	GLuint texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width2, height2;
	unsigned char* image2 = SOIL_load_image("./leaves.jpg", &width2, &height2, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image2);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.


	// ====================
	// Texture 3 (Player)
	// ====================
	GLuint texture3;
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width3, height3;
	unsigned char* image3 = SOIL_load_image("./Companion_Cube.png", &width3, &height3, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image3);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.


	// ====================
	// Texture 4 (DummyShadows)
	// ====================
	GLuint texture4;
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width4, height4;
	unsigned char* image4 = SOIL_load_image("./Shadow.png", &width4, &height4, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width4, height4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image4);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image4);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// ====================
	// Texture 5 (LightTexture)
	// ====================
	GLuint texture5;
	glGenTextures(1, &texture5);
	glBindTexture(GL_TEXTURE_2D, texture5); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width5, height5;
	unsigned char* image5 = SOIL_load_image("./Light.png", &width5, &height5, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width5, height5, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image5);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	//-- Audio Buffers --//
	//-- Audio Files Created using BFXR --//
	sf::SoundBuffer moveSoundBuffer;
	if (!moveSoundBuffer.loadFromFile("stone01.ogg"))
	{
		cout << "Failed to load 'stone01.ogg'" << endl;
	}
		

	sf::Sound moveSound;
	moveSound.setBuffer(moveSoundBuffer);

	//-- Loading the theme song to a buffer --//
	sf::Music mainTheme;
	if(!mainTheme.openFromFile("MainTheme.ogg"))
	{
		cout << "Failed to load 'MainTheme.ogg'" << endl;
	}

	mainTheme.setVolume(85);
	mainTheme.setLoop(true);
	mainTheme.play();

	//-- Main Program Loop --//
	while (!glfwWindowShouldClose(mainWindow))
	{
		if (freeformEnabled)
		{
			//-- Calculate the time between the last frame and the current one --//
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
		}
		
		//-- Poll the window for incoming events such as keyboard input or mouse operations --//
		glfwPollEvents();

		//-- Render --//
		//-- Clear the colorbuffer --//
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		//-- Activating "ourShader" --//
		ourShader.Use();

		//-- Camera/View transformations --//

		glm::mat4 view;
		if (freeformEnabled == false)
		{
			GLfloat yaw = 0.0f;
			GLfloat pitch = -45.0f;

			glm::vec3 front;
			front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			front.y = sin(glm::radians(pitch));
			front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(front);

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		}

		if (freeformEnabled == true)
		{
			view = glm::lookAt(freecameraPos, freecameraPos + freecameraFront, freecameraUp);
		}

		//-- Plane drawing Section --//

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
		//-- Projection --//
		glm::mat4 projection;
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		

		GLint planelightColorLoc = glGetUniformLocation(ourShader.Program, "lightColor");
		GLint planelightPosLoc = glGetUniformLocation(ourShader.Program, "lightPosArr");
		//GLint planelightDist = glGetUniformLocation(ourShader.Program, "lightDist");

		// Optimize here
		const static int lightArrSize = CWlightPosVect.size();
		glm::vec3 lightPosArray[5];

		for (int i = 0; i < 5; i++)
		{
			lightPosArray[i] = CWlightPosVect[i];
		}


		glUniform3fv(planelightPosLoc, 5, glm::value_ptr(lightPosArray[0]));
		//glUniform1f(planelightDist, distanceCalc(CWlightPosVect[0], playerPos));
		glUniform3f(planelightColorLoc, 1.0f, 1.0f, 0.4f);

		//-- Get the uniform locations --//
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		//-- Pass the matrices to the shader --//
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);




		//-- Calculation of each individual model matrix --//
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		//-- Player Render Section --//

		//-- Calculate player position as an offset of the camera --//

		playerPos = cameraPos - glm::vec3(-4.0f, 5.0f, 0.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glUniform1i(glGetUniformLocation(shaderPlayer.Program, "ourTexture1"), 0);

		shaderPlayer.Use();

		//-- Player Projection --//
		glm::mat4 Playerprojection;
		Playerprojection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		//-- Get the uniform locations --//
		
		GLint PlayerColorLoc = glGetUniformLocation(shaderPlayer.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderPlayer.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderPlayer.Program, "lightPos");
		GLint lightDist = glGetUniformLocation(shaderPlayer.Program, "lightDist");
		//-- Pass the matrices to the shader --//

		
		glUniform3f(PlayerColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 0.4f);


		//-- Determine the closest light and then apply shading according to the closest one --//
		float minDist = 10000000;
		int vectorLightLoc = 0;
		for (int i = 0; i < CWlightPosVect.size(); i++)
		{
			float distance = distanceCalc(CWlightPosVect[i], playerPos);
			if (distance <= minDist)
			{
				minDist = distance;
				vectorLightLoc = i;
			}
		}


		glUniform3f(lightPosLoc, CWlightPosVect[vectorLightLoc].x, CWlightPosVect[vectorLightLoc].y, CWlightPosVect[vectorLightLoc].z);
		glUniform1f(lightDist, distanceCalc(CWlightPosVect[vectorLightLoc], playerPos));

		GLint PlayermodelLoc = glGetUniformLocation(shaderPlayer.Program, "model");
		GLint PlayerviewLoc = glGetUniformLocation(shaderPlayer.Program, "view");
		GLint PlayerprojLoc = glGetUniformLocation(shaderPlayer.Program, "projection");
		glUniformMatrix4fv(PlayerviewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(PlayerprojLoc, 1, GL_FALSE, glm::value_ptr(Playerprojection));

		glBindVertexArray(PlayerVAO);
		



		//-- Calculation of each individual model matrix --//
		glm::mat4 Playermodel;
		
		Playermodel = glm::translate(Playermodel, playerPos);
		Playermodel = glm::rotate(Playermodel, forwardRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		Playermodel = glm::rotate(Playermodel, sidewardRotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(PlayermodelLoc, 1, GL_FALSE, glm::value_ptr(Playermodel));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//-- The movement animation routine along with camera movements --//
		if (!animEnabled && playerAnimationClock.getElapsedTime().asMilliseconds() > 5)
		{
			if (animDirection == 0)
			{
				cameraPos.x += cameraSpeed;
				forwardRotationAngle -= 0.1f;
			}

			else if (animDirection == 1)
			{
				cameraPos.x -= cameraSpeed;
				forwardRotationAngle += 0.1f;
			}

			else if (animDirection == 2)
			{
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				sidewardRotationAngle += 0.1f;
			}

			else if (animDirection == 3)
			{
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				sidewardRotationAngle -= 0.1f;
			}

			if (forwardRotationAngle >= 3.0f || forwardRotationAngle <= -3.0f)
			{
				animEnabled = true;
				forwardRotationAngle = 0.0f;
				moveSound.play();
			}

			else if (sidewardRotationAngle >= 3.0f || sidewardRotationAngle <= -3.0f)
			{
				animEnabled = true;
				sidewardRotationAngle = 0.0f;
				moveSound.play();
			}

			playerAnimationClock.restart();
		}


		//-- DEBUGGING SECTION --//
		//-- Testing Collision Basics --//
		/*
		for (int i = 0; i < treePosVect.size(); i++)
		{
			if (distanceCalc(treePosVect[i], playerPos) <= 0.5)
			{
				treePosVect.erase(treePosVect.begin() + i);
			}
		}
		*/

		//cout << playerPos.x << " " << playerPos.z << endl;
		//cout << cameraPos.x << " " << cameraPos.z << endl;
		//cout << endl;
		//-- DEBUGGING SECTION --//



		glBindVertexArray(0);

		//-- Tree rendering section --//

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shaderTree.Program, "ourTexture1"), 0);

		//-- Activating "ourShader" --//
		shaderTree.Use();

		//-- Tree Projection --//
		glm::mat4 Treeprojection;
		Treeprojection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		//-- Get the uniform locations --//
		GLint TreemodelLoc = glGetUniformLocation(shaderTree.Program, "model");
		GLint TreeviewLoc = glGetUniformLocation(shaderTree.Program, "view");
		GLint TreeprojLoc = glGetUniformLocation(shaderTree.Program, "projection");
		//-- Pass the matrices to the shader --//
		glUniformMatrix4fv(TreeviewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(TreeprojLoc, 1, GL_FALSE, glm::value_ptr(Treeprojection));

		glBindVertexArray(TreeVAO);

		objectYpos = sin(glfwGetTime());

		for (int i = 0; i < treePosVect.size(); i++)
		{
			//-- Calculation of each individual model matrix --//
			glm::mat4 Treemodel;
			Treemodel = glm::translate(Treemodel, treePosVect[i] + glm::vec3(0.0f, objectYpos + 1.0f, 0.0f));
			//-- The angle changes over time, acceleration is caused by the sinusoidal change over time altering rotation. --//
			glUniformMatrix4fv(TreemodelLoc, 1, GL_FALSE, glm::value_ptr(Treemodel));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		glBindVertexArray(0);


		//-- Shadow rendering section --//

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture4);
		glUniform1i(glGetUniformLocation(shaderShadow.Program, "ourTexture1"), 0);

		//-- Activating "ourShader" --//
		shaderShadow.Use();

		//-- Tree Projection --//
		glm::mat4 ShadowProjection;
		ShadowProjection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		//-- Get the uniform locations --//
		GLint ShadowModelLoc = glGetUniformLocation(shaderShadow.Program, "model");
		GLint ShadowViewloc = glGetUniformLocation(shaderShadow.Program, "view");
		GLint ShadowProjloc = glGetUniformLocation(shaderShadow.Program, "projection");
		//-- Pass the matrices to the shader --//
		glUniformMatrix4fv(ShadowViewloc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(ShadowProjloc, 1, GL_FALSE, glm::value_ptr(ShadowProjection));

		glBindVertexArray(ShadowVAO);

		for (int i = 0; i < treePosVect.size(); i++)
		{
			//-- Calculation of each individual model matrix --//
			glm::mat4 ShadowModel;
			ShadowModel = glm::translate(ShadowModel, glm::vec3(treePosVect[i][0], 0.001, treePosVect[i][2]));
			ShadowModel = glm::scale(ShadowModel, glm::vec3(1.3f - (objectYpos + 1.0f)/2, 1.0f, 1.3f - (objectYpos + 1.0f)/2));
			//-- The angle changes over time, acceleration is caused by the sinusoidal change over time altering rotation. --//
			glUniformMatrix4fv(ShadowModelLoc, 1, GL_FALSE, glm::value_ptr(ShadowModel));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);


		//-- Light rendering section --//

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture5);
		glUniform1i(glGetUniformLocation(shaderLight.Program, "ourTexture1"), 0);

		//-- Activating "ourShader" --//
		shaderLight.Use();

		//-- Light Projection --//
		glm::mat4 Lightprojection;
		Lightprojection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		//-- Get the uniform locations --//
		GLint LightmodelLoc = glGetUniformLocation(shaderLight.Program, "model");
		GLint LightviewLoc = glGetUniformLocation(shaderLight.Program, "view");
		GLint LightprojLoc = glGetUniformLocation(shaderLight.Program, "projection");
		//-- Pass the matrices to the shader --//
		glUniformMatrix4fv(LightviewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(LightprojLoc, 1, GL_FALSE, glm::value_ptr(Lightprojection));

		glBindVertexArray(LightCubeVAO);

		objectYpos = sin(glfwGetTime());

		for (int i = 0; i < CWlightPosVect.size(); i++)
		{
			//-- Calculation of each individual model matrix --//
			glm::mat4 Lightmodel;
			GLfloat radius = 0.25f;
			GLfloat lightX = sin(glfwGetTime()) * radius;
			GLfloat lightZ = cos(glfwGetTime()) * radius;
			Lightmodel = glm::translate(Lightmodel, CWlightPosVect[i] + glm::vec3(lightX, 1.0f, lightZ));
			Lightmodel = glm::rotate(Lightmodel, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
			//-- The angle changes over time, acceleration is caused by the sinusoidal change over time altering position. --//
			glUniformMatrix4fv(LightmodelLoc, 1, GL_FALSE, glm::value_ptr(Lightmodel));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		glBindVertexArray(0);


		//-- Swap the screen buffers --//
		glfwSwapBuffers(mainWindow);
	}
	//-- Terminate GLFW, clearing any resources allocated by GLFW. --//
	glfwTerminate();

	return 0;
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	GLfloat freecameraSpeed = 1.0f;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}



	if (key == GLFW_KEY_W)
	{
		if (animEnabled && !freeformEnabled)
		{
			animEnabled = false;
			animDirection = 0;
		}

		if (freeformEnabled)
		{
			freecameraPos += freecameraSpeed * freecameraFront;
		}
	}
		
	if (key == GLFW_KEY_S)
	{
		if (animEnabled && !freeformEnabled)
		{
			animEnabled = false;
			animDirection = 1;
		}

		if (freeformEnabled)
		{
			freecameraPos -= freecameraSpeed * freecameraFront;
		}
	}
		
	if (key == GLFW_KEY_A)
	{ 
		if (animEnabled && !freeformEnabled)
		{
			animEnabled = false;
			animDirection = 3;
		}

		if (freeformEnabled)
		{
			freecameraPos -= glm::normalize(glm::cross(freecameraFront, freecameraUp)) * freecameraSpeed;
		}
	}
		
	if (key == GLFW_KEY_D)
	{ 
		if (animEnabled && !freeformEnabled)
		{
			animEnabled = false;
			animDirection = 2;
		}

		if (freeformEnabled)
		{
			freecameraPos += glm::normalize(glm::cross(freecameraFront, freecameraUp)) * freecameraSpeed;
		}
	}
		
	//-- Camera Rotation Buttons  Q-E --//
	if (key == GLFW_KEY_Q)
	{
		
	}

	if (key == GLFW_KEY_E)
	{

	}

	if (key == GLFW_KEY_R)
	{

	}

	if (key == GLFW_KEY_P)
	{
		if (freeFormClock.getElapsedTime().asMilliseconds() > 1000)
		{
			freeformEnabled = !freeformEnabled;
			if (freeformEnabled)
			{
				cout << "Free Camera Movement Enabled" << endl;
			}

			else
			{
				cout << "Free Camera Movement Disabled" << endl;
			}

			//-- We slow down the user input to prevent repetitive swapping using a clock --//
			freeFormClock.restart();
		}
		
	}
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (freeformEnabled)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		GLfloat sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += 10.0f*xoffset;
		pitch += 10.0f*yoffset;

		/*
		if (pitch > 120.0f)
			pitch = 120.0f;
		if (pitch < -120.0f)
			pitch = -120.0f;
			*/

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		freecameraFront = glm::normalize(front);
	}
}

double distanceCalc(glm::vec3 pointOne, glm::vec3 pointTwo)
{
	double Delta = 0.0;

	Delta = sqrt(pow(pointOne.x - pointTwo.x, 2) + pow(pointOne.y - pointTwo.y, 2) + pow(pointOne.z - pointTwo.z, 2));

	return Delta;
}