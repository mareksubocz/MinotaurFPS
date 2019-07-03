#ifndef DRAW_MODEL_H
#define DRAW_MODEL_H

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>






void drawModel(GLuint &vertexbuffer, GLuint &uvbuffer, glm::mat4 &M,
               glm::mat4 &VP, GLuint &MatrixID, unsigned int vertexNumber, GLuint &Texture, GLuint &TextureID);









#endif // DRAW_MODEL_H
