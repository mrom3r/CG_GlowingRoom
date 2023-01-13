#include "playground.hpp"

#include <glfw3.h>

GLFWwindow *window;

#include <cstdio>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

#include <chrono>
#include <thread>

#include <common/shader.hpp>

int main() {
    //Initialize window
    bool windowInitialized = initializeWindow();
    if (!windowInitialized) return -1;

    //Initialize vertex buffer
    bool vertexBufferInitialized = initializeVertexBuffer();
    if (!vertexBufferInitialized) return -1;

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("../resources/SimpleVertexShader.vertexshader",
                            "../resources/SimpleFragmentShader.fragmentshader");

    // Get a handle for our "MVP" and "MV" uniforms and update them for initialization
    MatrixIDMV = glGetUniformLocation(programID, "MV");
    MatrixID = glGetUniformLocation(programID, "MVP");
    updateMVPTransformation();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    //start animation loop until escape key is pressed
    do {

        updateAnimationLoop();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    //Cleanup and close window
    cleanupVertexBuffer();
    glDeleteProgram(programID);
    closeWindow();

    return 0;
}

void updateAnimationLoop() {
    // use our shader
    glUseProgram(programID);

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // check if a key was pressed
    if (glfwGetKey(window, GLFW_KEY_F)) cameraZoom += 0.2;
    else if (glfwGetKey(window, GLFW_KEY_R)) cameraZoom -= 0.2;

    else if (glfwGetKey(window, GLFW_KEY_W)) rotationX += 0.02;
    else if (glfwGetKey(window, GLFW_KEY_S)) rotationX -= 0.02;

    else if (glfwGetKey(window, GLFW_KEY_Q)) rotationY += 0.02;
    else if (glfwGetKey(window, GLFW_KEY_E)) rotationY -= 0.02;

    else if (glfwGetKey(window, GLFW_KEY_D)) rotationZ += 0.02;
    else if (glfwGetKey(window, GLFW_KEY_A)) rotationZ -= 0.02;

    // Update the MVP transformation with the new values
    updateMVPTransformation();

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform and also the "MV" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(MatrixIDMV, 1, GL_FALSE, &MV[0][0]);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(texID, 0);


    renderingObject.DrawObject();

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool initializeWindow() {
    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "GlowingRoom", nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return false;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // background color
    glClearColor(0.0, 0.0, 0.3, 1.0);
    return true;
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
    // keep display dimension 16:9
    int game_width;
    int game_height;
    int half_gap_x{};
    int half_gap_y{};
    if (width > height) {
        // width larger
        game_width = {static_cast<int>(static_cast<float>(height) * (16.0f / 9.0f))};
        game_height = {height};
        half_gap_x = {(width - game_width) / 2};
    } else {
        // height larger
        game_width = {width};
        game_height = {static_cast<int>(static_cast<float>(width) * (9.0f / 16.0f))};
        half_gap_y = {(height - game_height) / 2};
    }

    glViewport(half_gap_x, half_gap_y, game_width, game_height);
}


bool updateMVPTransformation() {
    // projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 0.1f, 10000.0f);

    // camera matrix
    glm::mat4 view = glm::lookAt(
            glm::vec3(0, 0, cameraZoom), // Camera is at (0,0,0), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // model matrix
    glm::mat4 model = glm::mat4(1.0f); //start with identity matrix

    // rotation
    model = glm::rotate(model, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

    // translation
    model = glm::translate(model, glm::vec3(rotationX, rotationY, 0.0f));

    // model view projection
    MVP = projection * view * model; // Remember, matrix multiplication is the other way around
    MV = view * model; //We also need MV in the shader to transform the light position

    return true;
}

bool initializeVertexBuffer() {
    glGetUniformLocation(programID, "../resources/myTextureSampler");

    renderingObject = RenderingManager();
    renderingObject.InitializeVAO();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    loadOBJ("../resources/GlowingRoom.obj", vertices, uvs, normals);

    vertices.size() * sizeof(glm::vec3);

    renderingObject.SetVertices(vertices);
    renderingObject.SetNormals(normals);
    RenderingManager::computeTrianglesVertices(vertices, normals);

    renderingObject.textureSamplerID = glGetUniformLocation(programID, "myTextureSampler");
    std::vector<glm::vec2> uvBufferData = uvs;
    renderingObject.SetTexture(uvBufferData, "../resources/concrete.bmp");

    glGenBuffers(2, vertexBuffer); //generate two buffers, one for the vertices, one for the normals

    // fill first buffer (vertices)
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // fill second buffer (normals)
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    return true;
}


bool cleanupVertexBuffer() {
    // Cleanup VBO
    glDeleteBuffers(2, vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);
    return true;
}

bool closeWindow() {
    glfwTerminate();
    return true;
}