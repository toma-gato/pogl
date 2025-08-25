#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <random>

#include "Particle.hh"
#include "Star.hh"
#include "Vec3.hh"
#include "Comet.hh"

class Particle;
class Vec3;

class CometScene {
public:
    GLFWwindow* window;
    GLuint shaderProgram;
    GLuint particleShaderProgram;
    GLuint starShaderProgram;
    
    // VAO et VBO
    GLuint VAO, VBO;
    GLuint particleVAO, particleVBO;
    GLuint starVAO, starVBO;
    GLuint axisVAO, axisVBO;
    GLuint axisShaderProgram;
    
    // Données de la scène
    std::vector<Particle> particles;
    std::vector<Star> stars;
    std::vector<Comet> comets;
    
    // Caméra
    Vec3 cameraPos;
    Vec3 cameraTarget;
    Vec3 cameraUp;

    // Caméra controls
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 600.0f;
    float lastY = 400.0f;
    bool firstMouse = true;
    float cameraSpeed = 5.0f;
    
    // Temps
    float currentTime;
    float deltaTime;
    float lastTime;
    
    // Générateur de nombres aléatoires
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    

    CometScene();
    ~CometScene();
    
    void initWindow();
    void initOpenGL();
    
    GLuint compileShader(const char* source, GLenum type);
    GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);

    bool initShaders();
    void initGeometry();
    void initStars();
    
    void spawnParticle(const Comet& comet);

    void mouseCallback(double xpos, double ypos);
    void keyCallback(int key, int scancode, int action, int mods);
    
    void update();
    void render();
    void run();
    void cleanup();
};