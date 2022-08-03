#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <random>

#include "shader/shader.h"
#include "walk.hpp"

#define GRAPH_VERTICES  4

constexpr const unsigned int width = 1000, height = 1000;
constexpr const unsigned int graphWidth = 800, graphHeight = 800;
constexpr const float widthOffset = 1.0f / (float)graphWidth;
constexpr const float heightOffset = 1.0f / (float)graphHeight;
static GLFWwindow* handle = NULL;

// Dynamic globals
bool buildNewGraph = true;

void input_callback(GLFWwindow* handle, int key, int s, int action, int mods);
void reshape_callback(GLFWwindow* handle, int width, int height);
void destroy(GLFWwindow* handle, int exitcode);

typedef struct {
    GLfloat x, y, z;
} vertex_t;

vertex_t graphMargin[GRAPH_VERTICES] = {
    {-1.0f + widthOffset, -1.0f + heightOffset, 0.0f},
    {1.0f - widthOffset, -1.0f + heightOffset, 0.0f},
    {1.0f - widthOffset, 1.0f - heightOffset, 0.0f},
    {-1.0f + widthOffset, 1.0f - heightOffset, 0.0f} 
};

// Y-value changes based on where zero is
vertex_t zeroLine[2] {
    {-1.0f + widthOffset, 0.0f, 0.0f},
    {1.0f - widthOffset, 0.0f, 0.0f},
};

// Vertices of the graph 
vertex_t vertices[WALKLIMIT] = {0};

void reshape_callback(GLFWwindow* handle, int width, int height) {
    // Set the viewport to the entire window when resized
    glViewport(0, 0, width, height);
}

// ==========================
// Destroys the program
// ==========================
void destroy(GLFWwindow* handle, int exitcode) {
    glfwDestroyWindow(handle);
    glfwTerminate();
    exit(exitcode);
}

void input_callback(GLFWwindow* handle, int key, int s, int action, int mods) {
    switch (key) {
    case GLFW_KEY_R:
        buildNewGraph = true;
        break;
    case GLFW_KEY_ESCAPE:
        destroy(handle, 0);
        break;
    }
}

void buildGraph(walk& walkRef, vertex_t vertices[WALKLIMIT]) {
    for (unsigned int i = 0; i < walkRef.walkMax; i++) {
        float normX = 2.0f * i / walkRef.walkMax - 1.0f;
        float normY = 2.0f * (walkRef.data(i) - walkRef.min_y()) / (walkRef.max_y() - walkRef.min_y()) - 1.0f;
        vertex_t tmp = { normX, normY, 0.0f };
        vertices[i] = tmp;
    }
}

void buildZeroLine(walk& walkRef, vertex_t zeroLine[2]) {
    zeroLine[0].y = (2.0f * -walkRef.min_y()) / (walkRef.max_y() - walkRef.min_y()) - 1.0f;
    zeroLine[1].y = (2.0f * -walkRef.min_y()) / (walkRef.max_y() - walkRef.min_y()) - 1.0f;
}

void algo(walk& walkRef) {
    const float drift = 0.1f;
    const float stddev = 1.0f;
    float S0 = 24.84F;
    std::normal_distribution<> nd{0, 1};
    std::random_device rd{};
    std::mt19937 gen{rd()}; 
    walkRef.data(0) = S0;
    float beta = 0.50f;

    for (unsigned int i = 0; i < walkRef.walkMax - 1; i++) {
        float random = nd(gen);
        walkRef.data(i + 1) = walkRef.data(i) + (random * beta);
    }
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    // Set the window hints (version, GL profile, window)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create the window that returns a GLFWwindow pointer
    GLFWwindow* handle = glfwCreateWindow(width, height, "Random walk simulation", NULL, NULL);

    if (!handle) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Set the callbacks for keyboard, window resizing
    glfwSetFramebufferSizeCallback(handle, reshape_callback);
    glfwSetKeyCallback(handle, input_callback);

    // Make GLFW use our window, make glad load GL from GLFW, and set the swap interval to every frame
    glfwMakeContextCurrent(handle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    if (handle == NULL) destroy(handle, -1);

    shader line("line.vs", "line.fs");
    GLuint vao, vbo, graphVbo, graphVao, zeroLineVbo, zeroLineVao;
    glEnable(GL_DEPTH_TEST);
    glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &graphVao);
    glGenVertexArrays(1, &zeroLineVao);
    glGenBuffers(1, &zeroLineVbo);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &graphVbo);

    glBindBuffer(GL_ARRAY_BUFFER, graphVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(graphMargin), graphMargin, GL_STATIC_DRAW);

    glBindVertexArray(graphVao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)(0));
    glEnableVertexAttribArray(0);

    unsigned int graphXo = 100, graphYo = 100;
    glViewport(graphXo, graphYo, graphWidth, graphHeight);

    walk randwalk(1000, &algo);

    while (!glfwWindowShouldClose(handle)) {
        line.use();
        // Colored background and clear the buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (buildNewGraph) {
            randwalk.generate();
            buildGraph(randwalk, vertices);
            buildZeroLine(randwalk, zeroLine);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

            glBindVertexArray(vao);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)(0));
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, zeroLineVbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(zeroLine), zeroLine, GL_DYNAMIC_DRAW);

            glBindVertexArray(zeroLineVao);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)(0));
            glEnableVertexAttribArray(0);

            buildNewGraph = false;
        }

        glBindVertexArray(graphVao);
        glDrawArrays(GL_LINE_LOOP, 0, GRAPH_VERTICES);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, randwalk.walkMax);

        line.set_bool("dotted", true);
        glBindVertexArray(zeroLineVao);
        glDrawArrays(GL_LINES, 0, 2);
        line.set_bool("dotted", false);

        // Swap the buffer and poll any events that occur (calls to keyboard and mouse and window)
        glfwSwapBuffers(handle);
        glfwPollEvents(); 
    }

    return 0;
}