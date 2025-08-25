# Real-time OpenGL Comets and Particle System

This academic project from **POGL course** is a C++ project using OpenGL 4.5 to render a dynamic 3D space scene. The project features comets with trailing particles, a starfield, and interactive camera controls.

## Features
- Real-time 3D rendering with OpenGL.
- Comets with procedurally generated particle trails.
- Starfield background with brightness and color variations.
- Interactive camera controls (keyboard & mouse).
- Toggle camera movement and simulation updates.
- Mini compass overlay to indicate orientation.

## Controls
- **W / S / A / D**: Move the camera when free movement is enabled.
- **Mouse**: Look around when free movement is enabled.
- **C**: Toggle camera movement mode.
- **R**: Reset camera to initial position.
- **P**: Pause or resume simulation updates.
- **ESC**: Close the window.

---

## Requirements
- C++17 or newer
- OpenGL 4.5 compatible GPU
- GLFW
- GLEW
- CMake (optional, if you want to build with it)

---

## Project Structure

.  
├── Makefile  
├── README.md  
├── shaders/ # GLSL shaders for comets, particles, stars, and axes  
└── src/ # C++ source files  

---

## How to Build
1. Clone the repository:
   ```bash
   git clone git@github.com:toma-gato/pogl.git
   cd pogl
    ```

2. Build using Make:
    ```bash
    make
    ```

3. Run the program:
    ```bash
    ./pogl
    ```

---

## Authors
- Thomas Galateau
- Jules-Victor Lépinay
