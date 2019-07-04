#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <string>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <iostream>

#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <drawmodel.hpp>

#include <vector>

#include <bass.h>
#pragma comment(lib, "Bass.lib")



#define LABIRYNTH_NOTHING 0
#define LABIRYNTH_WALL 1
#define LABIRYNTH_GOAL 2
#define LABIRYNTH_LIGHT 3
int goalSet = 0;
int goalX = 5;
int goalY = 5;


int main( void )
{
    std::vector<std::vector<int> > labirynth(10, std::vector<int>(10));
    srand(time(0));
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1024, 768, "MinotaurFPS", NULL, NULL);
	if( window == NULL ){
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);


    BASS_Init(-1, 44100, 0, 0, NULL);
    BASS_SetVolume(1);




	//background color
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders( "vertex.glsl", "fragment.glsl" );

	// Get a handle for our MVP uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");


	//generate map
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

    if(!goalSet)
        labirynth[5][5] = LABIRYNTH_GOAL;
    }

    for(int i = 0; i < labirynth.size(); i++){
        for(int j = 0; j < labirynth[i].size(); j++)
            std::cout << labirynth[i][j] << " ";
        std::cout << std::endl;

    }

    std::cout << std::endl;






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

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);


	do{
        // Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		detectCollision(labirynth);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);



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

                glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
                glBufferData(GL_ARRAY_BUFFER, wallUvs.size() * sizeof(glm::vec2), &wallUvs[0], GL_STATIC_DRAW);


                glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
                glBufferData(GL_ARRAY_BUFFER, wallNormals.size() * sizeof(glm::vec3), &wallNormals[0], GL_STATIC_DRAW);


                drawModel(vertexbuffer, uvbuffer, normalbuffer, M, VP, MatrixID, ModelMatrixID, wallVertices.size(), wallTexture, TextureID);



            }



		}


        //==============================DRAW THE CHAIR==========================================
        if(goalSet){
           M = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              2*goalX,0.0f, 2*goalY, 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, chairVertices.size() * sizeof(glm::vec3), &chairVertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, chairUvs.size() * sizeof(glm::vec2), &chairUvs[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, chairNormals.size() * sizeof(glm::vec3), &chairNormals[0], GL_STATIC_DRAW);

        drawModel(vertexbuffer, uvbuffer, normalbuffer, M, VP, MatrixID, ModelMatrixID, chairVertices.size(), chairTexture, TextureID);

        }




        //==============================DRAW THE FLOOR==========================================
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, floorVertices.size() * sizeof(glm::vec3), &floorVertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, floorUvs.size() * sizeof(glm::vec2), &floorUvs[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, floorNormals.size() * sizeof(glm::vec3), &floorNormals[0], GL_STATIC_DRAW);


        drawModel(vertexbuffer, uvbuffer, normalbuffer, M, VP, MatrixID, ModelMatrixID, floorVertices.size(), floorTexture, TextureID);



        //==============================DRAW A GUN==========================================
        glClear(GL_DEPTH_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, gunVertices.size() * sizeof(glm::vec3), &gunVertices[0], GL_STATIC_DRAW);

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
        drawModel(vertexbuffer, uvbuffer, normalbuffer, M, P, MatrixID, ModelMatrixID, gunVertices.size(), gunTexture, TextureID);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		if(getPlayFootstep()){
            HSAMPLE sample;
            switch(rand() % 3){
                case 0:
                     sample = BASS_SampleLoad(false, "footstep0.ogg", 0, 0, 1, BASS_SAMPLE_MONO);
                     std::cout << "played0" << std::endl;
                    break;
                case 1:
                     sample = BASS_SampleLoad(false, "footstep1.ogg", 0, 0, 1, BASS_SAMPLE_MONO);
                     std::cout << "played1" << std::endl;
                    break;
                case 2:
                     sample = BASS_SampleLoad(false, "footstep2.ogg", 0, 0, 1, BASS_SAMPLE_MONO);
                     std::cout << "played2" << std::endl;
                    break;
            }

            HCHANNEL channel=BASS_SampleGetChannel(sample, FALSE);
            BASS_ChannelPlay(channel, FALSE);
		}

		if(getPlayGunshot()){
            HSAMPLE sample = BASS_SampleLoad(false, "gunshot.ogg", 0, 0, 1, BASS_SAMPLE_MONO);
            HCHANNEL channel=BASS_SampleGetChannel(sample, FALSE);
            BASS_ChannelPlay(channel, FALSE);
		}

		if(getPlayVictory()){
            HSAMPLE sample = BASS_SampleLoad(false, "victory.ogg", 0, 0, 1, BASS_SAMPLE_MONO);
            HCHANNEL channel=BASS_SampleGetChannel(sample, FALSE);
            BASS_ChannelPlay(channel, FALSE);
		}


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

