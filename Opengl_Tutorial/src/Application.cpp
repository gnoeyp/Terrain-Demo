#include "Camera.h"
#include "CubeMap.h"
#include "Fire.h"
#include "GLUtils.h"
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

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX;
float lastY;
bool isFirstMouse = true;
bool isDragging = false;

void renderQuad(unsigned int& vao, unsigned int& vbo)
{
	if (vao == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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

	std::cout << glGetString(GL_VERSION) << std::endl;

	Shader::Init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	Camera& camera = Camera::GetInstance();

	Shader skyboxShader("res/shaders/skybox.vert", "res/shaders/skybox.frag");

	std::vector<std::string> faces = {
		"res/textures/skybox_miramar/miramar_ft.tga",
		"res/textures/skybox_miramar/miramar_bk.tga",
		"res/textures/skybox_miramar/miramar_up.tga",
		"res/textures/skybox_miramar/miramar_dn.tga",
		"res/textures/skybox_miramar/miramar_rt.tga",
		"res/textures/skybox_miramar/miramar_lf.tga"
	};
	CubeMap cubeMap(faces);

	glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);
	glm::vec3 lightPos(0.0f, 50.0f, 0.0f);
	glm::vec3 ambient(0.5f, 0.5f, 0.5f);
	glm::vec3 diffuse(0.5f, 0.5f, 0.5f);
	glm::vec3 specular(1.0f, 1.0f, 1.0f);

	//Ground ground;
	Terrain terrain(
		"res/textures/iceland_heightmap.png",
		"res/textures/mud_cracked_dry/mud_cracked_dry_03_diff_1k.jpg",
		"res/textures/mud_cracked_dry/AnyConv.com__mud_cracked_dry_03_nor_gl_1k.png"
	);
	Model house("res/textures/middle-earth-house/AllHouse.obj");
	Model wood("res/textures/low_obj_1500/low_obj_1500.obj");
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
	matrixUbo.BindShader(Shader::HOUSE);

	dirLightUbo.BindShader(Shader::HOUSE);
	dirLightUbo.BindShader(Shader::HEIGHTMAP);


	// Instancing
	//Shader::FIRE->SetVec3f("u_CameraUp", camera.GetUp());
	//Shader::FIRE->SetVec3f("u_CameraFront", camera.GetFront());
	//Fire fire(3.0f, 9.75f, 5.2f);


	// Framebuffer
	Shader shaderBlur("res/shaders/blur.vert", "res/shaders/blur.frag");
	Shader shaderBloom("res/shaders/bloom.vert", "res/shaders/bloom.frag");

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	unsigned int sceneColorbuffers[2];
	glGenTextures(2, sceneColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, sceneColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, sceneColorbuffers[i], 0);
	}

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	unsigned int bloomFramebuffers[2];
	glGenFramebuffers(2, bloomFramebuffers);


	unsigned int textureColorbuffers[2];
	glGenTextures(2, textureColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFramebuffers[i]);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffers[i], 0);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int quadVAO = 0, quadVBO;


	while (!glfwWindowShouldClose(window))
	{

		// Calculate delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Update
		//fire.Update(deltaTime);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
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

		//fire.ImGuiRender();

		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader::HOUSE->Bind();

		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		matrixUbo.SetData(0, &projection[0][0]);
		matrixUbo.SetData(1, &view[0][0]);

		dirLightUbo.SetData(0, &lightDirection[0]);
		dirLightUbo.SetData(1, &ambient[0]);
		dirLightUbo.SetData(2, glm::value_ptr(diffuse));
		dirLightUbo.SetData(3, &specular[0]);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		Shader::HOUSE->SetMat4f("u_Model", model);

		Shader::HOUSE->SetVec3f("u_ViewPos", camera.GetPosition());

		house.Draw(*Shader::HOUSE);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 9.7f, 5.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		Shader::HOUSE->SetMat4f("u_Model", model);
		wood.Draw(*Shader::HOUSE);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.7f, 9.7f, 5.3f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-15.f), glm::vec3(1.0f, 0.0f, 0.0f));
		Shader::HOUSE->SetMat4f("u_Model", model);
		wood.Draw(*Shader::HOUSE);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

		Shader::BASIC->Bind();
		Shader::BASIC->SetMat4f("u_Model", model);

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

		//fire.Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// bloom

		bool firstIteration = true;
		bool horizontal = true;
		shaderBlur.Bind();

		for (unsigned int i = 0; i < 100; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, bloomFramebuffers[horizontal]);
			glActiveTexture(GL_TEXTURE0);
			shaderBlur.SetInt("u_Image", 0);
			shaderBlur.SetInt("u_Horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, firstIteration ? sceneColorbuffers[1] : textureColorbuffers[!horizontal]);
			renderQuad(quadVAO, quadVBO);
			firstIteration = false;
			horizontal = !horizontal;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderBloom.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneColorbuffers[0]);
		shaderBloom.SetInt("u_Scene", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffers[!horizontal]);
		shaderBloom.SetInt("u_BloomBlur", 1);
		renderQuad(quadVAO, quadVBO);

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