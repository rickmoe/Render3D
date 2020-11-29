#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

int main(void)
{
    const int WIDTH = 800;
    const int HEIGHT = 600;
    const float FOV = 90.0f;
    const float Z_NEAR = 1.0f;
    const float Z_FAR = 10.0f;
    const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Render 3D", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!\n";

    std::cout << glGetString(GL_VERSION) << "\n";

    {
        float vertices[] = {
            -0.5f, -0.5f,  1.0f,   // 0
             0.5f, -0.5f,  1.0f,   // 1
             0.5f,  0.5f,  1.0f,   // 2
            -0.5f,  0.5f,  1.0f,   // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(vertices, sizeof(vertices));

        VertexBufferLayout layout;
        layout.push<float>(3);
        va.addBuffer(vb, layout);

        IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 proj = glm::perspective(glm::radians(FOV / 2), ASPECT_RATIO, Z_NEAR, Z_FAR);

        Shader shader("res/shaders/Simple.shader");
        shader.bind();

        va.unbind();
        shader.unbind();
        vb.unbind();
        ib.unbind();

        Renderer renderer;

        float r = 0.5;
        float inc = 0.02;

        glm::vec3 camPos( 0.0f,  0.0f, -1.0f);
        glm::vec3 centeredPoint( 0.0f,  0.0f,  0.0f);
        glm::vec3 upVect( 0.0f,  1.0f,  0.0f);

        while (!glfwWindowShouldClose(window))
        {
            renderer.clear();

            shader.bind();
            shader.setUniform4f("u_color", 1.0f * r, 0.2f, 0.4f, 1.0f);

            glm::mat4 view = glm::lookAt(camPos, centeredPoint, upVect);
            glm::mat4 mvp = proj * view * model;
            shader.setUniformMat4f("u_mvp", mvp);

            renderer.draw(va, ib, shader);

            if (r > 1.0)
                inc = -0.02;
            else if (r < 0.5)
                inc = 0.02;
            r += inc;

            glfwSwapBuffers(window);

            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}