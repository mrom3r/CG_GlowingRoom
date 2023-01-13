#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <common/texture.hpp>
#include "RenderingManager.hpp"
#include <common/objloader.hpp>

// shader id
GLuint programID;

// vertex buffer variables
GLuint vertexBuffer[2];
GLuint VertexArrayID;

// MVP matrix
GLuint MatrixID;
glm::mat4 MVP;
GLuint MatrixIDMV;
glm::mat4 MV;

// texture
GLuint texID;
GLuint uvBuffer;
GLuint Texture;

RenderingManager renderingObject;

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals; // Won't be used at the moment.

float cameraZoom{35};

float rotationX{0.26};
float rotationY{-0.64};
float rotationZ{};

int main(); //<<< main function, called at startup
void updateAnimationLoop(); //<<< updates the animation loop
bool initializeWindow(); //<<< initializes the window using GLFW and GLEW
bool updateMVPTransformation(); //<<< updates the MVP transform with the current pose
bool initializeVertexBuffer(); //<<< initializes the vertex buffer array and binds it OpenGL
bool cleanupVertexBuffer(); //<<< frees all recources from the vertex buffer
bool closeWindow(); //<<< Closes the OpenGL window and terminates GLFW

#endif
