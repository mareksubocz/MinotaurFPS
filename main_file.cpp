// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "drawmodel.hpp"

#include <vector>

//Zwraca jedynki tam, gdzie maja byc szesciany
std::vector< std::vector<bool> > labirynt(int rozmiar){
    std::vector< std::vector<bool> > a(rozmiar,std::vector<bool>(rozmiar, int(rand()*2/RAND_MAX)));
    a[0][0] = a[rozmiar-1][rozmiar-1] = 0;
    return a;
}

int main( void )
{
    labirynt(4);
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "MinotaurFPS", NULL, NULL);
	if( window == NULL ){
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "vertex.glsl", "fragment.glsl" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");



	// Read our .obj file
	std::vector<glm::vec3> gunVertices;
	std::vector<glm::vec2> gunUvs;
	std::vector<glm::vec3> gunNormals; // Won't be used at the moment.
	loadOBJ("GUN.obj", gunVertices, gunUvs, gunNormals);

	// Read our .obj file
	std::vector<glm::vec3> chairVertices;
	std::vector<glm::vec2> chairUvs;
	std::vector<glm::vec3> chairNormals; // Won't be used at the moment.
	loadOBJ("CHAIR.obj", chairVertices, chairUvs, chairNormals);

	// Read our .obj file
	std::vector<glm::vec3> wallVertices;
	std::vector<glm::vec2> wallUvs;
	std::vector<glm::vec3> wallNormals; // Won't be used at the moment.
	loadOBJ("WALL.obj", wallVertices, wallUvs, wallNormals);

		// Read our .obj file
	std::vector<glm::vec3> floorVertices;
	std::vector<glm::vec2> floorUvs;
	std::vector<glm::vec3> floorNormals; // Won't be used at the moment.
	loadOBJ("FLOOR.obj", floorVertices, floorUvs, floorNormals);


	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	GLuint gunTexture = loadDDS("gun.dds");
	GLuint wallTexture = loadDDS("wall.dds");
	GLuint chairTexture = loadDDS("chair.dds");
	GLuint floorTexture = loadDDS("floor.dds");

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);


	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);


	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();

		glm::mat4 VP = getProjectionMatrix() * getViewMatrix();





		glm::mat4 M = glm::mat4(1.0);

        //==============================DRAW A GUN==========================================
        //fill vertex buffer with vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, gunVertices.size() * sizeof(glm::vec3), &gunVertices[0], GL_STATIC_DRAW);

        //same with UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, gunUvs.size() * sizeof(glm::vec2), &gunUvs[0], GL_STATIC_DRAW);


        drawModel(vertexbuffer, uvbuffer, M, VP, MatrixID, gunVertices.size(), gunTexture, TextureID);


        //==============================DRAW A CHAIR==========================================
        //fill vertex buffer with vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, chairVertices.size() * sizeof(glm::vec3), &chairVertices[0], GL_STATIC_DRAW);

        //same with UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, chairUvs.size() * sizeof(glm::vec2), &chairUvs[0], GL_STATIC_DRAW);

        drawModel(vertexbuffer, uvbuffer, M, VP, MatrixID, chairVertices.size(), chairTexture, TextureID);


        //==============================DRAW A WALL==========================================
        //fill vertex buffer with vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, wallVertices.size() * sizeof(glm::vec3), &wallVertices[0], GL_STATIC_DRAW);

        //same with UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, wallUvs.size() * sizeof(glm::vec2), &wallUvs[0], GL_STATIC_DRAW);

        drawModel(vertexbuffer, uvbuffer, M, VP, MatrixID, wallVertices.size(), wallTexture, TextureID);


        //==============================DRAW THE FLOOR==========================================
        //fill vertex buffer with vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, floorVertices.size() * sizeof(glm::vec3), &floorVertices[0], GL_STATIC_DRAW);

        //same with UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, floorUvs.size() * sizeof(glm::vec2), &floorUvs[0], GL_STATIC_DRAW);

        drawModel(vertexbuffer, uvbuffer, M, VP, MatrixID, floorVertices.size(), floorTexture, TextureID);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &gunTexture);
	glDeleteTextures(1, &wallTexture);
	glDeleteTextures(1, &chairTexture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

