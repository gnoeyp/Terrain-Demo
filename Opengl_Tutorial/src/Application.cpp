#include "Camera.h"
#include "CubeMap.h"
#include "Fire.h"
#include "GLUtils.h"
#include "Ground.h"
#include "Model.h"
#include "Shader.h"
#include "Terrain.h"
#include "UniformBlock.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <random>
#include <stb_image.h>

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 760;

//Camera camera(0.0f, 10.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX;
float lastY;
bool isFirstMouse = true;
bool isDragging = false;

void processInput(GLFWwindow* window)
{
	Camera& camera = Camera::GetInstance();
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
	Camera& camera = Camera::GetInstance();

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
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);

	Shader::Init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	Camera& camera = Camera::GetInstance();

	Shader skyboxShader("res/shaders/skybox.vert", "res/shaders/skybox.frag");
	//Shader groundShader("res/shaders/basic.vert", "res/shaders/basic.frag");

	std::vector<std::string> faces = {
		"res/textures/skybox_miramar/miramar_ft.tga",
		"res/textures/skybox_miramar/miramar_bk.tga",
		"res/textures/skybox_miramar/miramar_up.tga",
		"res/textures/skybox_miramar/miramar_dn.tga",
		"res/textures/skybox_miramar/miramar_rt.tga",
		"res/textures/skybox_miramar/miramar_lf.tga"
	};
	//CubeMap cubeMap("res/textures/skybox.png");
	CubeMap cubeMap(faces);

	glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);
	glm::vec3 lightPos(-0.2f, -1.0f, -0.3f);
	glm::vec3 ambient(0.5f, 0.5f, 0.5f);
	glm::vec3 diffuse(0.5f, 0.5f, 0.5f);
	glm::vec3 specular(1.0f, 1.0f, 1.0f);

	//Ground ground;
	Terrain terrain(
		"res/textures/iceland_heightmap.png",
		"res/textures/mud_cracked_dry/mud_cracked_dry_03_diff_1k.jpg",
		"res/textures/mud_cracked_dry/AnyConv.com__mud_cracked_dry_03_nor_gl_1k.png"
	);
	//Model ourModel("res/textures/backpack/backpack.obj");

	int terrainTexture = 0;

	std::vector<unsigned int> sizes = { sizeof(glm::mat4), sizeof(glm::mat4) };
	UniformBlock matrixUbo({ sizeof(glm::mat4), sizeof(glm::mat4) }, "u_Matrices");
	UniformBlock dirLightUbo(
		// base alignment of vec3 is 16
		{ 16, 16, 16, 16 },
		"u_DirLight"
	);

	matrixUbo.BindShader(Shader::HEIGHTMAP);
	matrixUbo.BindShader(Shader::BASIC);
	matrixUbo.BindShader(Shader::BASIC_TEXTURE);
	//matrixUbo.BindShader(groundShader);

	dirLightUbo.BindShader(Shader::BASIC_TEXTURE);
	dirLightUbo.BindShader(Shader::HEIGHTMAP);


	// Instancing
	Shader::FIRE->SetVec3f("u_CameraUp", camera.GetUp());
	Shader::FIRE->SetVec3f("u_CameraFront", camera.GetFront());
	Fire fire;

	while (!glfwWindowShouldClose(window))
	{
		// Calculate delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Update
		fire.Update(deltaTime);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Text("Hello, world %d", 123);
		if (ImGui::Button("Save"))
			std::cout << "Save button preessed" << std::endl;
		ImGui::ColorEdit3("Ambient", &ambient[0]);
		ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
		ImGui::ColorEdit3("Specular", &specular[0]);
		ImGui::SliderFloat("Light position X", &lightPos.x, -100.0f, 100.0f);
		ImGui::SliderFloat("Light position Y", &lightPos.y, -100.0f, 100.0f);
		ImGui::SliderFloat("Light position Z", &lightPos.z, -100.0f, 100.0f);
		ImGui::RadioButton("Terrain texture 1", &terrainTexture, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Terrain texture 2", &terrainTexture, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Terrain texture 3", &terrainTexture, 2);

		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader::BASIC_TEXTURE->Bind();

		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		matrixUbo.SetData(0, &projection[0][0]);
		matrixUbo.SetData(1, &view[0][0]);

		dirLightUbo.SetData(0, &lightDirection[0]);
		dirLightUbo.SetData(1, &ambient[0]);
		dirLightUbo.SetData(2, glm::value_ptr(diffuse));
		dirLightUbo.SetData(3, &specular[0]);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		Shader::BASIC_TEXTURE->SetMat4f("u_Model", model);

		Shader::BASIC_TEXTURE->SetVec3f("u_ViewPos", camera.GetPosition());

		//ourModel.Draw(*Shader::BASIC_TEXTURE);

		Shader::BASIC->Bind();
		Shader::BASIC->SetMat4f("u_Model", model);

		//ground.Draw(*Shader::BASIC);

		Shader::HEIGHTMAP->Bind();
		Shader::HEIGHTMAP->SetMat4f("u_Model", model);
		Shader::HEIGHTMAP->SetVec3f("u_ViewPos", camera.GetPosition());

		Shader::HEIGHTMAP->SetInt("u_TextureMethodType", terrainTexture);
		Shader::HEIGHTMAP->SetVec3f("u_FogColor", 0.35f, 0.46f, 0.56f);
		Shader::HEIGHTMAP->SetVec3f("u_LightPos", lightPos);

		terrain.Draw(*Shader::HEIGHTMAP);

		skyboxShader.Bind();
		skyboxShader.SetMat4f("u_Proj", projection);
		skyboxShader.SetMat4f("u_View", glm::mat4(glm::mat3(camera.GetViewMatrix())));
		cubeMap.Draw(skyboxShader);

		fire.Draw();

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