#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>
#include <shader/shader.hpp>
#include <stb_image/stb_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// #define DEBUG

#define WIDTH 800
#define HEIGHT 600
#define ZOOM_FACTOR 0.1f

#define JULIA_C 0.5667f
#define JULIA_P -0.5f

#define CENTER_INIT_X 0.7f
#define CENTER_INIT_Y 0.0f
#define SCALE_INIT 3.0f

#define GLSL_VERSION "#version 330 core"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void reset();

float current_width = (float) WIDTH;
float current_height = (float) HEIGHT;

float center_x = CENTER_INIT_X, center_y = CENTER_INIT_Y;
float scale = SCALE_INIT;
uint32_t iterations = 1000;
double prev_x, prev_y;
float delta_x, delta_y;
float tmp_center_x, tmp_center_y;
float p_x = 0.0f, p_y = 0.0f;
float c_x = 0.0f, c_y = 0.0f;
bool mandelbrot = true;
static uint8_t selectedFractal;

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

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

    Shader shader("../shaders/vert.shader", "../shaders/frag.shader");

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
    unsigned char *data = stbi_load("../textures/warm.png", &width, &height, &nrChannels, 0);
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
    while (!glfwWindowShouldClose(window)) {
        /* Start the Dear ImGui frame */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Fractal Generator");

        if (ImGui::RadioButton("Mandelbrot", selectedFractal == 0)) {
            selectedFractal = 0;
            mandelbrot = true;
            reset();
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Julia", selectedFractal == 1)) {
            selectedFractal = 1;
            mandelbrot = false;
            c_x = 0.5667f;
            c_y = 0.0f;
            p_x = -0.5f;
            p_y = 0.0f;
            reset();
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Phoenix", selectedFractal == 2)) {
            selectedFractal = 2;
            mandelbrot = false;
            c_x = -0.7f;
            c_y = 0.27015;
            p_x = 0.0f;
            p_y = 0.0f;
            reset();
        }

        if (selectedFractal == 2) {
            ImGui::Text("c");
            ImGui::SliderFloat("c_x", &c_x, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::InputFloat("c_x##TextBox", &c_x, 0.0f, 0.0f, "%.3f");
            ImGui::SliderFloat("c_y", &c_y, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::InputFloat("c_y##TextBox", &c_y, 0.0f, 0.0f, "%.3f");

            ImGui::Text("p");
            ImGui::SliderFloat("p_x", &p_x, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::InputFloat("p_x##TextBox", &p_x, 0.0f, 0.0f, "%.3f");
            ImGui::SliderFloat("p_y", &p_y, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::InputFloat("p_y##TextBox", &p_y, 0.0f, 0.0f, "%.3f");
        }

        ImGui::End();
        ImGui::Render();

        /* Render here */
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.setFloat("scale", scale);
        shader.setVec2("center", center_x, center_y);
        shader.setInt("iterations", iterations);
        shader.setFloat("ratio", current_width / current_height);
        shader.setVec2("p", p_x, p_y);
        shader.setVec2("c", c_x, c_y);
        shader.setBool("mandelbrot", mandelbrot);
        shader.use();
        glBindVertexArray(VAO);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Render the ImGui UI */
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
#ifdef DEBUG
    printf("Scale: %.10fx\n", 1.0f / scale);
#endif
}

void reset() {
    center_x = CENTER_INIT_X, center_y = CENTER_INIT_Y;
    scale = SCALE_INIT;
}
