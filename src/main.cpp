#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>
#include <shader/shader.hpp>
#include <stb_image/stb_image.h>

#define WIDTH 800
#define HEIGHT 600
#define ZOOM_FACTOR 0.1f

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

float current_width = (float) WIDTH;
float current_height = (float) HEIGHT;

float center_x = 0.7f, center_y = 0.0f;
float scale = 2.2f;
uint32_t iterations = 1000;
double prev_x, prev_y;
float delta_x, delta_y;
float tmp_center_x, tmp_center_y;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Fractal generator", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    float vertices[] = {
        //positions         // texture coordinates
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
       -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
       -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    Shader shader("../shaders/shader.vert", "../shaders/shader.frag");

    uint32_t VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // copy our vertex data to a vertex buffer for opengl to use
	uint32_t VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    uint32_t EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
	glEnableVertexAttribArray(0);   // enable this in layer 0

    // texture shenanigans
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../textures/cold.png", &width, &height, &nrChannels, 0);
    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "ERROR::TEXTURE::TEXTURE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    stbi_image_free(data);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(sizeof(float)*3));
	glEnableVertexAttribArray(1);   // enable this in layer 1

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.setFloat("scale", scale);
        shader.setFloat("center_x", center_x);
        shader.setFloat("center_y", center_y);
        shader.setInt("iterations", iterations);
        shader.setFloat("ratio", current_width / current_height);
        shader.use();
        glBindVertexArray(VAO);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    current_width = (float) width;
    current_height = (float) height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
            if (action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            break;

        default: break;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &prev_x, &prev_y);
        tmp_center_x = center_x;
        tmp_center_y = center_y;
    }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        delta_x = xpos - prev_x;
        delta_y = prev_y - ypos;
        // convert from screen space to math space
        delta_x *= (2.0f * scale / current_width);
        delta_y *= (2.0f * scale / current_height);
        // apply translation
        center_x = tmp_center_x + delta_x;
        center_y = tmp_center_y + delta_y;
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    scale *= 1 - (float) yoffset * ZOOM_FACTOR;
}
