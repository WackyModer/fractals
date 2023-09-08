#include <iostream>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

// Vertex shader source code
const char* vertexShaderSource = R"(
    #version 430 core
    layout (location = 0) in vec2 aPos;
    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    }
)";

// Fragment shader source code
const char* fragmentShaderSource = R"(
#version 430 core
out vec4 FragColor;

uniform double zoom;

uniform double x_pos;
uniform double y_pos;

uniform int maxiter;

uniform int winsize;

int mandel(double creal, double cimag){
    int iter = 0;
    
    double zreal = 0.0;
    double zimag = 0.0;
    
    double xreal = 1.0;
    double ximag = 1.0;


    while ((zreal*zreal+zimag*zimag) < 4.0 && iter < maxiter) {
        xreal = zreal;
        ximag = zimag;

        zreal = (xreal*xreal - ximag*ximag) + creal;
        zimag = (xreal*ximag + ximag*xreal) + cimag;

        iter++;

        if (zreal == xreal && zimag == ximag) {
            return maxiter;
        }
        //return maxiter;
    }

    return iter;
}


void main()
{
    // Get the screen-space coordinates of the current pixel
    vec2 pixelCoords = gl_FragCoord.xy;

    // Calculate the normalized coordinates (range [0, 1])
   
    //vec2 normalizedCoords = pixelCoords / vec2(800,600); // screenSize should be a uniform vec2

    // Set the color based on the pixel's position
    //vec4 color = vec4(normalizedCoords.x, normalizedCoords.y, cos(currentTime*3), 1.0);


    double real = double(pixelCoords.x - winsize / 2.0) / double(zoom) / double(winsize)+double(x_pos);
    double imag = double(pixelCoords.y - winsize / 2.0) / double(zoom) / double(winsize)+double(y_pos);

    int meit = mandel(real, imag);


    float r;
    float g;
    float b;

    if(meit == maxiter){
        r = 0.0;
        g = 0.0;
        b = 0.0;
    }else{
        r = (sin(meit * 0.1) * 0.5 + 0.5);
        g = (cos(meit * 0.1) * 0.5 + 0.5);
        b = 1.0;
    }


    vec4 color = vec4(r,g,b, 1.0);

    FragColor = color;
}
)";

double currentFrameTime;

double x_pos = -0.75;
double y_pos = 0;

double zoomf = 0.25;

int max_it = 100;

int screensize = 600;

std::string title = "OpenGL Mandelbrot Set | 100 iterations";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{


    // Process input
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

#pragma region movement_n_zoom


    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        zoomf *= 2;
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        zoomf /= 2;
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        y_pos += 0.25 * (1 / zoomf);
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        y_pos -= 0.25 * (1 / zoomf);
    }


    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        x_pos -= 0.25 * (1 / zoomf);
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        x_pos += 0.25 * (1 / zoomf);
    }

#pragma endregion

#pragma region iters

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        max_it -= 100;

        title = "OpenGL Mandelbrot Set | ";
        title += std::to_string(max_it);
        title += " iterations";

        glfwSetWindowTitle(window, title.c_str());
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        max_it += 100;

        title = "OpenGL Mandelbrot Set | ";
        title += std::to_string(max_it);
        title += " iterations";

        glfwSetWindowTitle(window, title.c_str());
    }

#pragma endregion

#pragma region screensize


    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        screensize -= 100;

        glfwSetWindowSize(window, screensize, screensize);
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        screensize += 100;

        glfwSetWindowSize(window, screensize, screensize);
    }

#pragma endregion
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(600, 600, "OpenGL Mandelbrot Set | 100 iterations", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Vertex data for a fullscreen quad
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    // Create Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }



    // Create shader program and link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    // Delete shader objects (no longer needed)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Specify vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // Set the desired frame rate (e.g., 30 FPS)
    const double targetFPS = 30.0;
    const double frameTime = 1.0 / targetFPS;

    // Get the uniform location for currentTime

    int currentZoomLocation = glGetUniformLocation(shaderProgram, "zoom");

    int currentX_PosLocation = glGetUniformLocation(shaderProgram, "x_pos");
    int currentY_PosLocation = glGetUniformLocation(shaderProgram, "y_pos");

    int currentMaxItLocation = glGetUniformLocation(shaderProgram, "maxiter");


    int currentScreenSizeLocation = glGetUniformLocation(shaderProgram, "winsize");



    glfwSetKeyCallback(window, key_callback);

    int norm_width = 600;

    while (!glfwWindowShouldClose(window)) {
        // Calculate the time taken by the previous frame
        static double prevFrameTime = glfwGetTime();
        double currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - prevFrameTime;
        prevFrameTime = currentFrameTime;


        // Frame capping logic
        if (deltaTime < frameTime) {
            // Calculate the remaining time to meet the desired frame rate
            double sleepTime = frameTime - deltaTime;

            // Wait for the desired frame time to elapse
            while (glfwGetTime() - currentFrameTime < sleepTime) {
                // Poll for events to prevent the application from becoming unresponsive
                glfwPollEvents();
            }
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate the current time
        currentFrameTime = glfwGetTime();

        // Use the shader program
        glUseProgram(shaderProgram);


        glUniform1i(currentMaxItLocation, static_cast<int>(max_it));
        glUniform1i(currentScreenSizeLocation, static_cast<int>(screensize));

        glUniform1d(currentZoomLocation, static_cast<double>(zoomf));

        glUniform1d(currentX_PosLocation, static_cast<double>(x_pos));
        glUniform1d(currentY_PosLocation, static_cast<double>(y_pos));



        // Bind VAO to render the fullscreen quad
        glBindVertexArray(VAO);

        // Draw the quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();

        int newWidth, newHeight;
        glfwGetFramebufferSize(window, &newWidth, &newHeight);

        if (newWidth != norm_width) {
            norm_width = newWidth;

            // Update the OpenGL viewport and projection matrix
            glViewport(0, 0, norm_width, norm_width);

            // You may also want to update your projection matrix here
            // For example, for an orthographic projection:
            // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight), -1.0f, 1.0f);
            // glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        }
    }


    // Cleanup and terminate
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}