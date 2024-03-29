#include <GLFW/glfw3.h>
extern GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include<iostream>
#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

glm::vec3 position = glm::vec3( -5, -5, -5 );

glm::vec3 getPosition(){
    return position;
}

float lastGunshot = 0;
int playGunshot = 0;
int getPlayGunshot(){
    return playGunshot;
}
float lastFootstep = 0;
int playFootstep = 0;
int getPlayFootstep(){
    return playFootstep;
}

int playVictory = 0;
int getPlayVictory(){
    return playVictory;
}




float horizontalAngle = 3.14f;

float verticalAngle = 0.0f;


float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;
bool inAir = true;
float currSpeedUp = 0.0f;
float prevPosX = position.x;
float prevPosZ = position.z;

void detectCollision(std::vector<std::vector<int> > labirynth){
    float buffer = 0.2;
    for(int i = 0; i < labirynth.size(); i++)
        for(int j = 0; j < labirynth[i].size(); j++)
            if(labirynth[i][j] == 1
               && position.x <= 2*i + 1 + buffer
               && position.x >= 2*i - 1 - buffer
               && position.z <= 2*j + 1 + buffer
               && position.z >= 2*j - 1 - buffer){
                position.x = prevPosX;
                position.z = prevPosZ;
                break;
            }
    prevPosX = position.x;
    prevPosZ = position.z;
}



void computeMatricesFromInputs(){

	static double lastTime = glfwGetTime();


	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);


	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	glfwSetCursorPos(window, 1024/2, 768/2);


	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );
	if(verticalAngle > 1.5f) verticalAngle = 1.5f;
	if(verticalAngle < -1.5f) verticalAngle = -1.5f;


	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);


	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);

	glm::vec3 nullOutVertical = glm::vec3(1,0,1);
	glm::vec3 floor = glm::vec3(0,1,0);

    glm::vec3 up(0, 1, 0);


    int moved = 0;
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += glm::normalize(direction * nullOutVertical) * deltaTime * speed;
		moved = 1;
	}

	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= glm::normalize(direction * nullOutVertical) * deltaTime * speed;
		moved = 1;
	}

	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * speed;
		moved = 1;
	}

	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
		moved = 1;
	}

	playFootstep = 0;
	if(moved && lastFootstep + 1 < glfwGetTime()){
        playFootstep = 1;
        lastFootstep = glfwGetTime();
	}


    if(glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS && inAir == false){
        inAir = true;
        currSpeedUp = speed;
    }
    //falling
    if(inAir == true){
        currSpeedUp -= speed/30;
        position += up * deltaTime * currSpeedUp;
        if(position.y <= 1.0)
            inAir = false;
    }
    if(position.y < 1.0) position.y = 1;

    //shooting
    float pixel;
    int width, height;
    float buffer = 0.00001;
    playGunshot = 0;
    playVictory = 0;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && lastGunshot + 1 < glfwGetTime()){
        lastGunshot = glfwGetTime();

        playGunshot = 1;
        glfwGetWindowSize(window, &width, &height);
        glReadPixels(width/2, height/2,1,1, GL_GREEN, GL_FLOAT, &pixel);

        if( (pixel <= 0.313726 + buffer && pixel >= 0.313726 - buffer) || pixel >= 1.0 - buffer){
            std::cout<<"WIN"<<std::endl;
            playVictory = 1;
        }

    }

	ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	ViewMatrix       = glm::lookAt(
								position,
								position+direction,
								up
						   );


	lastTime = currentTime;
}
