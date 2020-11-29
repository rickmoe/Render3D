#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Renderer.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

void setCamVel(glm::vec3* currCamVel, const glm::vec3& camVel);
void adjustCamVel(glm::vec3* currCamVel, const glm::vec3& adjustVect);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
glm::vec3 camVel(0.0f, 0.0f, 0.0f);
glm::vec2 camRotVel(0.0f, 0.0f);
glm::vec2 camFacing(0.0f, 0.0f);

int main(void)
{
    const int WIDTH = 800;
    const int HEIGHT = 600;
    const float FOV = 90.0f;
    const float Z_NEAR = 1.0f;
    const float Z_FAR = 1000.0f;
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
        /*
        float vertices[] = {
            -25.0f,  0.0f, -25.0f,
             25.0f,  0.0f, -25.0f,
             25.0f,  0.0f,  25.0f,
            -25.0f,  0.0f,  25.0f,
        };
        */
        float vertices[] = {
            -25.0f,  0.0f,  25.0f, -5.0f, -2.0f,
             25.0f,  0.0f,  25.0f,  5.0f, -2.0f,
             25.0f, 10.0f,  25.0f,  5.0f,  2.0f,
            -25.0f, 10.0f,  25.0f, -5.0f,  2.0f,
             25.0f,  0.0f, -25.0f, -5.0f, -2.0f,
            -25.0f,  0.0f, -25.0f,  5.0f, -2.0f,
            -25.0f, 10.0f, -25.0f,  5.0f,  2.0f,
             25.0f, 10.0f, -25.0f, -5.0f,  2.0f,
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4,
            1, 4, 7,
            7, 2, 1,
            5, 0, 3,
            3, 6, 5,
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(vertices, sizeof(vertices));

        VertexBufferLayout layout;
        layout.push<float>(3);
        layout.push<float>(2);
        va.addBuffer(vb, layout);

        IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 proj = glm::perspective(glm::radians(FOV / 2), ASPECT_RATIO, Z_NEAR, Z_FAR);

        Shader shader("res/shaders/Simple.shader");
        shader.bind();
        
        Texture texture("res/textures/Tile.png");
        texture.bind(0);
        shader.setUniform1i("u_texture", 0);

        va.unbind();
        shader.unbind();
        vb.unbind();
        ib.unbind();
        
        Renderer renderer;

        float r = 0.5;
        float inc = 0.02;

        glm::vec3 camPos( 0.0f,  2.0f,  0.0f);
        glm::vec3 centeredPoint( 0.0f,  0.0f,  0.0f);
        glm::vec3 upVect( 0.0f,  1.0f,  0.0f);

        glfwSetKeyCallback(window, keyCallback);

        while (!glfwWindowShouldClose(window))
        {
            renderer.clear();

            shader.bind();
            shader.setUniform4f("u_color", 1.0f * r, 1.0f * r, 1.0f * r, 1.0f);

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

            camPos += camVel;
            camFacing = camFacing + camRotVel;
            camFacing[0] = fmod(camFacing[0], 360.0f);
            camFacing[0] = abs(camFacing[0]) > 180.0f ? camFacing[0] - 360.0f * abs(camFacing[0]) / camFacing[0] : camFacing[0];
            camFacing[1] = std::fmaxf(-90.0f, std::fminf(90.0, camFacing[1]));
            centeredPoint = camPos + glm::vec3(-glm::sin(glm::radians(camFacing[0])), glm::sin(glm::radians(camFacing[1])), glm::cos(glm::radians(camFacing[0])));
        }
    }

    glfwTerminate();
    return 0;
}

void setCamVel(glm::vec3* currCamVel, const glm::vec3& camVel)
{
    *currCamVel = camVel;
}

void adjustCamVel(glm::vec3* currCamVel, const glm::vec3& adjustVect)
{
    *currCamVel += adjustVect;
}

void adjustCamVelRelative(glm::vec3* currCamVel, const glm::vec2& currCamFacing, const glm::vec3& adjustVect)
{
    glm::vec3 addVect(-sin(glm::radians(currCamFacing[0])) * adjustVect[2] - sin(glm::radians(currCamFacing[0] - 90)) * adjustVect[0], 
        adjustVect[1], cos(glm::radians(currCamFacing[0])) * adjustVect[2] + cos(glm::radians(currCamFacing[0] - 90)) * adjustVect[0]);
    *currCamVel += addVect;
}

void setCamRotVel(glm::vec2* currCamRotVel, const glm::vec2& camRotVel)
{
    *currCamRotVel = camRotVel;
}

void adjustCamRotVel(glm::vec2* currCamRotVel, const glm::vec2& adjustRotVect)
{
    *currCamRotVel += adjustRotVect;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const float MOVE_SPEED = 0.1f;
    int moveKeys[6] = { GLFW_KEY_W , GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT };
    glm::vec3 moveVects[6] = { glm::vec3(0.0f, 0.0f, MOVE_SPEED), glm::vec3(0.0f, 0.0f, -MOVE_SPEED), glm::vec3(-MOVE_SPEED, 0.0f, 0.0f), 
        glm::vec3(MOVE_SPEED, 0.0f, 0.0f), glm::vec3(0.0f, MOVE_SPEED, 0.0f), glm::vec3(0.0f, -MOVE_SPEED, 0.0f) };
    for (int i = 0; i < sizeof(moveKeys) / sizeof(int); i++)
    {
        if (key == moveKeys[i])
        {
            if (action == GLFW_PRESS)
            {
                adjustCamVelRelative(&camVel, camFacing, moveVects[i]);
            }
            else if (action == GLFW_RELEASE)
            {
                adjustCamVelRelative(&camVel, camFacing, -moveVects[i]);
            }
        }
    }

    const float TURN_SPEED = 2.0f;
    int turnKeys[4] = { GLFW_KEY_E, GLFW_KEY_Q, GLFW_KEY_R , GLFW_KEY_F };
    glm::vec2 turnVects[4] = { glm::vec2(-TURN_SPEED, 0.0f), glm::vec2(TURN_SPEED, 0.0f), glm::vec2(0.0f, TURN_SPEED), glm::vec2(0.0f, -TURN_SPEED) };
    for (int i = 0; i < sizeof(turnKeys) / sizeof(int); i++)
    {
        if (key == turnKeys[i])
        {
            if (action == GLFW_PRESS)
            {
                adjustCamRotVel(&camRotVel, turnVects[i]);
            }
            else if (action == GLFW_RELEASE)
            {
                adjustCamRotVel(&camRotVel, -turnVects[i]);
            }
        }
    }
}