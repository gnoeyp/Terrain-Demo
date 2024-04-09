#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <iostream>
#include <stb_image.h>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

Camera camera(0, 0, 1);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX;
float lastY;
bool isFirst = true;

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.MoveLeft(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.MoveRight(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.MoveFront(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.MoveBack(deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (isFirst)
	{
		lastX = xpos;
		lastY = ypos;
		isFirst = false;
		return;
	}
	float dYaw = 0.01f * (xpos - lastX);
	float dPitch = 0.01f * (lastY - ypos);
	lastX = xpos;
	lastY = ypos;

	camera.Rotate(dYaw, dPitch);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	Model ourModel("res/textures/battleship/Obj/space battleship lowpoly.obj");
	//Model ourModel("res/textures/backpack.obj");

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Bind();

		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		shader.SetMat4f("u_Proj", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		shader.SetMat4f("u_Model", model);
		glm::mat4 view = camera.GetViewMatrix();
		shader.SetMat4f("u_View", view);

		ourModel.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}