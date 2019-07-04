// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ctime>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <iostream>

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "drawmodel.hpp"

#include <vector>



#define LABIRYNTH_NOTHING 0
#define LABIRYNTH_WALL 1
#define LABIRYNTH_GOAL 2
#define LABIRYNTH_LIGHT 3
int goalSet = 0;
int goalX = 5;
int goalY = 5;
GLint lightList [3][25];


int main( void )
{
    std::vector<std::vector<int> > labirynth(10, std::vector<int>(10));
    srand(time(0));
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


	//generate map
    int lights = 0;
    for(int i = 0; i < labirynth.size(); i++){
        for(int j = 1; j < labirynth[i].size(); j++)
            if(((int)rand())%4==0) //25% chance for a block to be blocked
                labirynth[i][j] = LABIRYNTH_WALL;
            else if(!goalSet && ((int)rand())%15==0){
                labirynth[i][j] = LABIRYNTH_GOAL;
                goalSet = 1;
                goalX = i;
                goalY = j;

            }
            else if((int)rand()%5 == 0){ //20% chance for light to happen
                labirynth[i][j] = LABIRYNTH_LIGHT;
                lightList[0][lights] = 2*i;
                lightList[1][lights] = 2;
                lightList[2][lights] = 2*j;
                lights++;
            }

    if(!goalSet)
        labirynth[5][5] = LABIRYNTH_GOAL;
    }

    for(int i = 0; i < labirynth.size(); i++){
        for(int j = 0; j < labirynth[i].size(); j++)
            std::cout << labirynth[i][j] << " ";
        std::cout << std::endl;

    }

    std::cout << std::endl;

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 25; j++){
            std::cout << lightList[i][j] << " ";

        }
        std::cout << std::endl;

    }




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


	GLuint LightsXID = glGetUniformLocation(programID, "lightLocationsX");
	GLuint LightsYID = glGetUniformLocation(programID, "lightLocationsY");
	GLuint LightsZID = glGetUniformLocation(programID, "lightLocationsZ");

	glUniform1iv(LightsXID, 25, lightList[0]);
	glUniform1iv(LightsYID, 25, lightList[1]);
	glUniform1iv(LightsZID, 25, lightList[2]);


	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);


	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);


	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		detectCollision(labirynth);

		glm::mat4 VP = getProjectionMatrix() * getViewMatrix();

		glm::mat4 M = glm::mat4(1.0);

        for(int i = 0; i < labirynth.size(); i++){
            for(int j = 0; j < labirynth[i].size(); j++){
                if(labirynth[i][j] != LABIRYNTH_WALL)
                    continue;
                M = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              2*i,0.0f, 2*j, 1.0f);

                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, wallVertices.size() * sizeof(glm::vec3), &wallVertices[0], GL_STATIC_DRAW);

                //same with UV buffer
                glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
                glBufferData(GL_ARRAY_BUFFER, wallUvs.size() * sizeof(glm::vec2), &wallUvs[0], GL_STATIC_DRAW);


                glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
                glBufferData(GL_ARRAY_BUFFER, wallNormals.size() * sizeof(glm::vec3), &wallNormals[0], GL_STATIC_DRAW);


                drawModel(vertexbuffer, uvbuffer, normalbuffer, M, VP, MatrixID, wallVertices.size(), wallTexture, TextureID);



            }



		}




        //same with UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, gunUvs.size() * sizeof(glm::vec2), &gunUvs[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, gunNormals.size() * sizeof(glm::vec3), &gunNormals[0], GL_STATIC_DRAW);


        drawModel(vertexbuffer, uvbuffer, normalbuffer, M, VP, MatrixID, gunVertices.size(), gunTexture, TextureID);


        //==============================DRAW A CHAIR==========================================
        //fill vertex buffer with vertex data
        if(goalSet){
           M = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              2*goalX,0.0f, 2*goalY, 1.0f);

          glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, chairVertices.size() * sizeof(glm::vec3), &chairVertices[0], GL_STATIC_DRAW);

        //same with UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, chairUvs.size() * sizeof(glm::vec2), &chairUvs[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, chairNormals.size() * sizeof(glm::vec3), &chairNormals[0], GL_STATIC_DRAW);

        drawModel(vertexbuffer, uvbuffer, normalbuffer, M, VP, MatrixID, chairVertices.size(), chairTexture, TextureID);

        }




        //==============================DRAW THE FLOOR==========================================
        //fill vertex buffer with vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, floorVertices.size() * sizeof(glm::vec3), &floorVertices[0], GL_STATIC_DRAW);

        //same with UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, floorUvs.size() * sizeof(glm::vec2), &floorUvs[0], GL_STATIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, floorNormals.size() * sizeof(glm::vec3), &floorNormals[0], GL_STATIC_DRAW);


        drawModel(vertexbuffer, uvbuffer, normalbuffer, M, VP, MatrixID, floorVertices.size(), floorTexture, TextureID);



        //==============================DRAW A GUN==========================================
        //clear depth buffer so gun is always on top
        glClear(GL_DEPTH_BUFFER_BIT);
        //fill vertex buffer with vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, gunVertices.size() * sizeof(glm::vec3), &gunVertices[0], GL_STATIC_DRAW);

        //same with UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, gunUvs.size() * sizeof(glm::vec2), &gunUvs[0], GL_STATIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, gunNormals.size() * sizeof(glm::vec3), &gunNormals[0], GL_STATIC_DRAW);


        glm::vec3 pos = getPosition();


        M = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      glm::sin(pos.x*2)*0.05, glm::sin(pos.z*4)*0.05, -0.5f, 1.0f);
        glm::mat4 P = getProjectionMatrix();
        drawModel(vertexbuffer, uvbuffer, normalbuffer, M, P, MatrixID, gunVertices.size(), gunTexture, TextureID);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup
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

