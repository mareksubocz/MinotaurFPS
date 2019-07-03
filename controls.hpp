#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <vector>
void detectCollision(std::vector<std::vector<int> >);
void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getPosition();
#endif
