#include "Camera.h"
#include "CubeMap.h"
#include "Ground.h"
#include "Model.h"
#include "Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <stb_image.h>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

Camera camera(0, 0, 1);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX;
float lastY;
bool isFirstMouse = true;
bool isDragging = false;

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
	if (!isDragging) return;
	if (isFirstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		isFirstMouse = false;
		return;
	}
	float sensitivity = 0.1f;
	float dYaw = sensitivity * (lastX - xpos);
	float dPitch = sensitivity * (ypos - lastY);
	lastX = xpos;
	lastY = ypos;

	camera.Rotate(dYaw, dPitch);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		isDragging = true;
		isFirstMouse = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		isDragging = false;
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	Shader::Init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	//Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	Shader skyboxShader("res/shaders/skybox.vert", "res/shaders/skybox.frag");
	Shader groundShader("res/shaders/basic.vert", "res/shaders/basic.frag");
	//Model ourModel("res/textures/backpack/backpack.obj");
	std::vector<std::string> faces = {
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg",
	};
	CubeMap cubeMap(faces);

	glm::vec3 ambient(0.5f, 0.5f, 0.5f);
	glm::vec3 diffuse(0.5f, 0.5f, 0.5f);
	glm::vec3 specular(1.0f, 1.0f, 1.0f);

	Ground ground;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Text("Hello, world %d", 123);
		if (ImGui::Button("Save"))
			std::cout << "Save button preessed" << std::endl;
		ImGui::ColorEdit3("Ambient", &ambient[0]);
		ImGui::ColorEdit3("Diffuse", &diffuse[0]);
		ImGui::ColorEdit3("Specular", &specular[0]);

		processInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Shader::BASIC_TEXTURE.Bind();

		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		//Shader::BASIC_TEXTURE.SetMat4f("u_Proj", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//Shader::BASIC_TEXTURE.SetMat4f("u_Model", model);
		//Shader::BASIC_TEXTURE.SetMat4f("u_View", camera.GetViewMatrix());

		//Shader::BASIC_TEXTURE.SetVec3f("viewPos", camera.GetPosition());
		//Shader::BASIC_TEXTURE.SetVec3f("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		//Shader::BASIC_TEXTURE.SetVec3f("dirLight.ambient", ambient);
		//Shader::BASIC_TEXTURE.SetVec3f("dirLight.diffuse", diffuse);
		//Shader::BASIC_TEXTURE.SetVec3f("dirLight.specular", specular);

		//ourModel.Draw(shader);
		Shader::BASIC->Bind();
		Shader::BASIC->SetMat4f("u_Proj", projection);
		Shader::BASIC->SetMat4f("u_Model", model);
		Shader::BASIC->SetMat4f("u_View", camera.GetViewMatrix());

		ground.Draw(*Shader::BASIC);

		skyboxShader.Bind();
		skyboxShader.SetMat4f("u_Proj", projection);
		skyboxShader.SetMat4f("u_View", glm::mat4(glm::mat3(camera.GetViewMatrix())));
		cubeMap.Draw(skyboxShader);


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}