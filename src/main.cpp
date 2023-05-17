#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader/shader.hpp>

#define WIDTH 800
#define HEIGHT 600

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Fractal generator", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    float vertices[] = {
       -1.0f, -1.0f, 0.0f, // top left
        1.0f, -1.0f, 0.0f, // top right
       -1.0f,  1.0f, 0.0f, // bottom left
        1.0f,  1.0f, 0.0f  // bottom right
    };  

    uint32_t indices[] = {
        0, 1, 2,    // first triangle
        1, 2, 3     // second triangle
    };

    Shader shader("../shaders/shader.vs", "../shaders/shader.fs");

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
	glEnableVertexAttribArray(0);   // enable this in layer 0

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        shader.use();
        glBindVertexArray(VAO);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
