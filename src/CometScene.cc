#include "CometScene.hh"
#include "Mat4.hh"
#include "Utils.hh"

#include <iostream>
#include <algorithm>
#include <cmath>

GLuint program_id;
bool update_status = true;
bool camera_movement = true;

CometScene::CometScene() : 
    cameraPos(0, 0, 5),
    cameraTarget(0, 0, 0),
    cameraUp(0, 1, 0),
    currentTime(0),
    deltaTime(0),
    lastTime(0),
    rng(std::random_device{}()),
    dist(-1.0f, 1.0f)
{
    // Init des comètes avec position, vitesse, taille et couleur
    comets.emplace_back(Vec3(-15, 0, 0), Vec3(2, 0.5f, 0), 0.15f, Vec3(0.2f, 0.4f, 0.9f));
    comets.emplace_back(Vec3(-20, 2, 1), Vec3(2.5f, -0.3f, 0.2f), 0.1f, Vec3(0.9f, 0.2f, 0.4f));
    comets.emplace_back(Vec3(-18, -1, -2), Vec3(1.8f, 0.2f, -0.4f), 0.12f, Vec3(0.4f, 0.9f, 0.2f));
    comets.emplace_back(Vec3(-22, 1, 3), Vec3(2.2f, -0.5f, 0.1f), 0.14f, Vec3(0.6f, 0.3f, 0.8f));
    comets.emplace_back(Vec3(-25, 0, 0), Vec3(1.5f, 0.4f, 0), 0.18f, Vec3(0.1f, 0.7f, 0.9f));

    initWindow();
    initOpenGL();
    initShaders();
    initGeometry();
    initStars();
}

CometScene::~CometScene() {
    cleanup();
}

void CometScene::initWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(1200, 800, "Comète et Particules", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    
    glfwSetFramebufferSizeCallback(window, []([[maybe_unused]]GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    // Configuration des inputs clavier et souris
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, this);

    // Callback de la souris
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        CometScene* scene = static_cast<CometScene*>(glfwGetWindowUserPointer(window));
        scene->mouseCallback(xpos, ypos);
    });

    // Callback du clavier
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        CometScene* scene = static_cast<CometScene*>(glfwGetWindowUserPointer(window));
        scene->keyCallback(key, scancode, action, mods);
    });
}

void CometScene::initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Couleur du fond pour simuler l'espace
    glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
}

GLuint CometScene::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    
    return shader;
}

GLuint CometScene::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint compile_status = GL_TRUE;
    glGetProgramiv(program, GL_LINK_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        GLint log_size;
        char *shader_log;
        shader_log = (char*)std::malloc(log_size + 1);
        if (shader_log != 0) {
            glGetProgramInfoLog(program, log_size, &log_size, shader_log);
            std::cerr << "Program linking failed: " << shader_log << std::endl;
            std::free(shader_log);
        }
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

bool CometScene::initShaders() {
    std::string shaderDir = "shaders/";
    std::string cometVertexShader = load(shaderDir + "comet_vertex.glsl");
    std::string cometFragmentShader = load(shaderDir + "comet_fragment.glsl");
    std::string particleVertexShader = load(shaderDir + "particle_vertex.glsl");
    std::string particleFragmentShader = load(shaderDir + "particle_fragment.glsl");
    std::string starVertexShader = load(shaderDir + "star_vertex.glsl");
    std::string starFragmentShader = load(shaderDir + "star_fragment.glsl");
    std::string axisVertexShader = load(shaderDir + "axis_vertex.glsl");
    std::string axisFragmentShader = load(shaderDir + "axis_fragment.glsl");

    shaderProgram = createShaderProgram(cometVertexShader.c_str(), cometFragmentShader.c_str());
    particleShaderProgram = createShaderProgram(particleVertexShader.c_str(), particleFragmentShader.c_str());
    starShaderProgram = createShaderProgram(starVertexShader.c_str(), starFragmentShader.c_str());
    axisShaderProgram = createShaderProgram(axisVertexShader.c_str(), axisFragmentShader.c_str());

    if (!shaderProgram || !particleShaderProgram || !starShaderProgram || !axisShaderProgram) {
        std::cerr << "Failed to create shader programs" << std::endl;
        return false;
    }

    return true;
}

void CometScene::initGeometry() {
    std::vector<float> vertices;
    int segments = 16;

    // Génération des vertices pour la sphère de la comète
    for (int y = 0; y <= segments; y++) {
        float phi = M_PI * (float)y / segments;
        
        for (int x = 0; x <= segments; x++) {
            float theta = 2.0f * M_PI * (float)x / segments;
            
            // Calcul des coordonnées sphériques
            float xPos = cos(theta) * sin(phi);
            float yPos = cos(phi);
            float zPos = sin(theta) * sin(phi);
            
            // Mise à l'échelle pour la taille de la comète
            vertices.push_back(xPos * 0.15f);
            vertices.push_back(yPos * 0.15f);
            vertices.push_back(zPos * 0.15f);
            
            // Normales (même direction que la position pour une sphère)
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }
    
    std::vector<unsigned int> indices;
    for (int y = 0; y < segments; y++) {
        for (int x = 0; x < segments; x++) {
            indices.push_back(y * (segments + 1) + x);
            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + x + 1);
            indices.push_back(y * (segments + 1) + x + 1);
            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back((y + 1) * (segments + 1) + x + 1);
        }
    }

    // Boussole
    float axisVertices[] = {
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // X axis start (rouge)
        5.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // X axis end
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // Y axis start (vert)
        0.0f, 5.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // Y axis end
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // Z axis start (bleu)
        0.0f, 0.0f, 5.0f,  0.0f, 0.0f, 1.0f   // Z axis end
    };
    
    GLuint EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Comète
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position + Normale = 6
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    

    // Particules
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);
    
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    
    // Position + Color + Size = 7
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Étoiles
    glGenVertexArrays(1, &starVAO);
    glGenBuffers(1, &starVBO);

    // Boussole
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // Position + Couleur = 6
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void CometScene::initStars() {
    stars.clear();
    for (int i = 0; i < 1000; i++) {
        Star star;
        star.position = Vec3(
            dist(rng) * 50,
            dist(rng) * 50,
            dist(rng) * 50 - 25
        );
        star.brightness = 0.5f + dist(rng) * 0.5f;
        star.color = Vec3(
            0.8f + dist(rng) * 0.2f,
            0.8f + dist(rng) * 0.2f,
            0.9f + dist(rng) * 0.1f
        );
        stars.push_back(star);
    }
    
    std::vector<float> starData;
    for (const auto& star : stars) {
        starData.push_back(star.position.x);
        starData.push_back(star.position.y);
        starData.push_back(star.position.z);
        starData.push_back(star.brightness);
        starData.push_back(star.color.x);
        starData.push_back(star.color.y);
        starData.push_back(star.color.z);
    }
    
    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, starData.size() * sizeof(float), starData.data(), GL_STATIC_DRAW);
    
    // Position + Brightness + Color = 7
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void CometScene::spawnParticle(const Comet& comet) {
    Particle particle;
    particle.position = comet.position + Vec3(
        -0.15f + dist(rng) * 0.15f,
        dist(rng) * 0.15f,
        dist(rng) * 0.15f
    );
    particle.velocity = Vec3(
        -comet.velocity.x * (0.3f + dist(rng) * 0.3f),
        dist(rng) * 0.15f - 0.075f,
        dist(rng) * 0.15f - 0.075f
    );
    particle.life = particle.maxLife = 8.0f + dist(rng) * 7.0f;
    particle.size = 3.0f + dist(rng) * 3.0f;

    float intensity = 0.8f + dist(rng) * 0.2f;
    particle.color = Vec3(
        comet.color.x * intensity,
        comet.color.y * intensity,
        comet.color.z * intensity
    );
    
    particles.push_back(particle);
}

void CometScene::update() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastTime;
    lastTime = currentFrame;
    
    if (camera_movement) {
        float radius = 15.0f;
        cameraPos.x = cos(lastTime * 0.15f) * radius;
        cameraPos.z = sin(lastTime * 0.15f) * radius;
        cameraPos.y = 5.0f;
    }
    else {
        float cameraSpeed = 5.0f * deltaTime;
        Vec3 forward = (cameraTarget - cameraPos).normalize();
        Vec3 right = forward.cross(cameraUp).normalize();
    
        // Mouvement de la caméra
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { // Z
            cameraPos = cameraPos + forward * cameraSpeed;
            cameraTarget = cameraTarget + forward * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { // S
            cameraPos = cameraPos - forward * cameraSpeed;
            cameraTarget = cameraTarget - forward * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { // D
            cameraPos = cameraPos + right * cameraSpeed;
            cameraTarget = cameraTarget + right * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { // Q
            cameraPos = cameraPos - right * cameraSpeed;
            cameraTarget = cameraTarget - right * cameraSpeed;
        }
    }

    for (auto& comet : comets) {
        comet.position = comet.position + comet.velocity * deltaTime;
        
        // Réinitialiser la comète si elle sort de l'écran
        if (comet.position.x > 15) {
            comet.position = Vec3(-15, dist(rng) * 5, dist(rng) * 5);
            comet.velocity = Vec3(2 + dist(rng), dist(rng) * 0.5f, dist(rng) * 0.5f);
        }
        
        // Générer des particules pour cette comète
        for (int i = 0; i < 500; i++) {
            spawnParticle(comet);
        }
    }
    
    // Mise à jour des particules
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), 
            [this](Particle& p) {
                p.update(deltaTime);
                return !p.isAlive();
            }),
        particles.end()
    );
    
}

void CometScene::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    // Matrices de vue et projection
    Mat4 view = Mat4::lookAt(cameraPos, cameraTarget, cameraUp);
    Mat4 projection = Mat4::perspective(45.0f * M_PI / 180.0f, (float)width / height, 0.1f, 100.0f);
    
    // Rendu des étoiles
    glUseProgram(starShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(starShaderProgram, "view"), 1, GL_FALSE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(starShaderProgram, "projection"), 1, GL_FALSE, projection.m);
    
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(starVAO);
    glDrawArrays(GL_POINTS, 0, stars.size());
    
    // Rendu des comètes
    glUseProgram(shaderProgram);
    for (const auto& comet : comets) {
        Mat4 model = Mat4::translate(comet.position);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection.m);
        glUniform3f(glGetUniformLocation(shaderProgram, "cometColor"), comet.color.x, comet.color.y, comet.color.z);
        
        int segments = 16;
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, segments * segments * 6, GL_UNSIGNED_INT, 0);
    }
    
    // Rendu des particules
    if (!particles.empty()) {
        std::vector<float> particleData;
        for (const auto& particle : particles) {
            particleData.push_back(particle.position.x);
            particleData.push_back(particle.position.y);
            particleData.push_back(particle.position.z);
            particleData.push_back(particle.color.x);
            particleData.push_back(particle.color.y);
            particleData.push_back(particle.color.z);
            particleData.push_back(particle.size);
        }
        
        glUseProgram(particleShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(particleShaderProgram, "view"), 1, GL_FALSE, view.m);
        glUniformMatrix4fv(glGetUniformLocation(particleShaderProgram, "projection"), 1, GL_FALSE, projection.m);
        
        glBindVertexArray(particleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
        glBufferData(GL_ARRAY_BUFFER, particleData.size() * sizeof(float), particleData.data(), GL_DYNAMIC_DRAW);
        
        glDrawArrays(GL_POINTS, 0, particles.size());
    }

    // Rendu de la boussole
    glDisable(GL_DEPTH_TEST);

    glUseProgram(axisShaderProgram);
    Mat4 fixedView;
    fixedView.identity();

    Mat4 orthoProj = Mat4::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    glUniformMatrix4fv(glGetUniformLocation(axisShaderProgram, "view"), 1, GL_FALSE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(axisShaderProgram, "projection"), 1, GL_FALSE, orthoProj.m);
    glUniform2f(glGetUniformLocation(axisShaderProgram, "screenPos"), 0.8f, 0.8f);
    glUniform1f(glGetUniformLocation(axisShaderProgram, "scale"), 0.01f);
    
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);

    glEnable(GL_DEPTH_TEST);

    glfwSwapBuffers(window);
}

void CometScene::mouseCallback(double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    cameraPos.x = cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f) * 15.0f;
    cameraPos.y = sin(pitch * M_PI / 180.0f) * 15.0f;
    cameraPos.z = sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f) * 15.0f;
}

void CometScene::keyCallback(int key, [[maybe_unused]]int scancode, int action, [[maybe_unused]]int mods) {
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        firstMouse = true;
        yaw = -90.0f;
        pitch = 0.0f;
        cameraPos = Vec3(0, 0, 5);
        cameraTarget = Vec3(0, 0, 0);
        cameraUp = Vec3(0, 1, 0);
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        update_status = !update_status;
        lastTime = glfwGetTime();
        if (update_status) {
            std::cout << "Update enabled." << std::endl;
        } else {
            std::cout << "Update disabled." << std::endl;
        }
    }
    else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        camera_movement = !camera_movement;
        firstMouse = true;
        yaw = -90.0f;
        pitch = 0.0f;
        cameraPos = Vec3(0, 0, 5);
        cameraTarget = Vec3(0, 0, 0);
        cameraUp = Vec3(0, 1, 0);
        if (camera_movement) {
            std::cout << "Camera movement enabled." << std::endl;
        } else {
            std::cout << "Camera movement disabled." << std::endl;
        }
    }
}

void CometScene::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        
        if (update_status) {
            update();
        }
        render();
    }
}

void CometScene::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &particleVAO);
    glDeleteBuffers(1, &particleVBO);
    glDeleteVertexArrays(1, &starVAO);
    glDeleteBuffers(1, &starVBO);
    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &axisVBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(particleShaderProgram);
    glDeleteProgram(starShaderProgram);
    glDeleteProgram(axisShaderProgram);
    
    glfwTerminate();
}
