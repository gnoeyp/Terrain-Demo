#include "Camera.h"
#include "CubeMap.h"
#include "Fire.h"
#include "GLUtils.h"
#include "House.h"
#include "Model.h"
#include "Shader.h"
#include "Terrain.h"
#include "UniformBlock.h"
#include "Wood.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <imGuIZMO.quat/imGuIZMOquat.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <random>
#include <stb_image.h>

const unsigned int DEPTH_MAP_INDEX = 20;

unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX;
float lastY;
bool isFirstMouse = true;
bool isDragging = false;


unsigned int framebuffer;
unsigned int sceneColorbuffers[2];
unsigned int rbo;
unsigned int bloomFramebuffers[2];
unsigned int textureColorbuffers[2];
unsigned int quadVAO = 0, quadVBO;

void InitFramebuffers();

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
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteFramebuffers(2, bloomFramebuffers);
	glDeleteTextures(2, sceneColorbuffers);
	glDeleteTextures(2, textureColorbuffers);
	glDeleteRenderbuffers(1, &rbo);
	windowWidth = width;
	windowHeight = height;
	InitFramebuffers();
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


void InitFramebuffers()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(2, sceneColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, sceneColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, sceneColorbuffers[i], 0);
	}

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(2, bloomFramebuffers);

	glGenTextures(2, textureColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFramebuffers[i]);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffers[i], 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);

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
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	float ambient = 0.2f;
	float shininess = 0.5f;

	//Ground ground;
	Terrain terrain(
		"res/textures/perlin.png",
		//"res/textures/mud_cracked_dry/mud_cracked_dry_03_diff_1k.jpg",
		//"res/textures/mud_cracked_dry/AnyConv.com__mud_cracked_dry_03_nor_gl_1k.png"
		//"res/textures/sand_muddy/TCom_Sand_Muddy2_2x2_1K_albedo.jpg",
		//"res/textures/sand_muddy/TCom_Sand_Muddy2_2x2_1K_normal.jpg"
		//"res/textures/GroundDirtRocky020/GroundDirtRocky020_COL_2K.jpg",
		//"res/textures/GroundDirtRocky020/GroundDirtRocky020_NRM_2K.jpg",
		//"res/textures/gray_rocks/gray_rocks_diff_4k.jpg",
		//"res/textures/gray_rocks/gray_rocks_nor_gl_4k.jpg",
		"res/textures/grass/grass01.jpg",
		"res/textures/GroundDirtRocky020/GroundDirtRocky020_NRM_2K.jpg",
		"res/textures/rdiffuse.jpg",
		"res/textures/rnormal.jpg",
		glm::scale(
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
			glm::vec3(1.0f, 1.0f, 1.0f)
		)
	);

	float houseOffsetX = 0.0f;
	float houseOffsetY = 0.0f;

	float woodOffsetX = 0.0f;
	float woodOffsetY = 0.0f;

	House house;

	Wood wood;

	UniformBlock matrixUbo(
		{ 0, sizeof(glm::mat4) },
		{ sizeof(glm::mat4), sizeof(glm::mat4) }, "u_Matrices");
	UniformBlock dirLightUbo(
		{
			0, // color
			16, // direction
			28, // ambient
			32 // shininess
		},
		{
			12, // color
			12, // direction
			4, // ambient
			4 // shininess
		},
		"u_DirLight"
	);
	UniformBlock lightSpaceMatrixUbo({ 0 }, { sizeof(glm::mat4) }, "u_LightSpaceMatrix");

	matrixUbo.BindShader(Shader::HEIGHTMAP);
	matrixUbo.BindShader(Shader::BASIC);
	matrixUbo.BindShader(Shader::HOUSE);
	matrixUbo.BindShader(Shader::SKYBOX);

	dirLightUbo.BindShader(Shader::HOUSE);
	dirLightUbo.BindShader(Shader::HEIGHTMAP);

	lightSpaceMatrixUbo.BindShader(Shader::HEIGHTMAP);
	lightSpaceMatrixUbo.BindShader(Shader::HOUSE);


	// Instancing
	Shader::FIRE->SetVec3f("u_CameraUp", camera.GetUp());
	Shader::FIRE->SetVec3f("u_CameraFront", camera.GetFront());
	Fire fire(3.0f, 9.75f, 5.2f);


	// Framebuffer
	Shader shaderBlur("res/shaders/blur.vert", "res/shaders/blur.frag");
	Shader shaderBloom("res/shaders/bloom.vert", "res/shaders/bloom.frag");

	InitFramebuffers();

	quat qRot = quat(1.0f, 0.0f, 0.0f, 0.0f);
	vec3 dir;

	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float near_plane = 0.0f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	Shader debugDepthShader("res/shaders/debugDepth.vert", "res/shaders/debugDepth.frag");

	bool showDepthBuffer = false;

	unsigned int frameCount = 0;
	float previousTime = 0.0f;
	unsigned int fps = 0;

	while (!glfwWindowShouldClose(window))
	{
		frameCount++;
		// Calculate delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Update
		fire.Update(deltaTime);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Text("FPS: %d", fps);
		ImGui::ColorEdit3("Color", &lightColor[0]);
		ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &shininess, 0.0f, 10.0f);
		ImGui::Text("Light Direction");
		if (ImGui::gizmo3D("", dir))
			lightDirection = glm::normalize(glm::vec3(dir.x, dir.y, dir.z));
		ImGui::Checkbox("Depth buffer", &showDepthBuffer);

		if (currentFrame - previousTime > 1.0)
		{
			previousTime = currentFrame;
			fps = frameCount;
			frameCount = 0;
		}


		fire.ImGuiRender();
		terrain.ImGuiRender();
		house.ImGuiRender();
		wood.ImGuiRender();

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 lightView = glm::lookAt(
			camera.GetPosition() - 10.0f * lightDirection,
			camera.GetPosition(),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		matrixUbo.SetData(0, &lightProjection[0][0]);
		matrixUbo.SetData(1, &lightView[0][0]);
		dirLightUbo.SetData(0, &lightColor[0]);
		dirLightUbo.SetData(1, &lightDirection[0]);
		dirLightUbo.SetData(2, &ambient);
		dirLightUbo.SetData(3, &shininess);
		glm::mat4 idMatrix = glm::mat4(1.0f);
		lightSpaceMatrixUbo.SetData(0, &idMatrix[0][0]);

		house.Draw(*Shader::HOUSE);
		wood.Draw();

		terrain.Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, windowWidth, windowHeight);

		if (showDepthBuffer)
		{
			debugDepthShader.Bind();
			debugDepthShader.SetInt("depthMap", 0);
			debugDepthShader.SetFloat("near_plane", near_plane);
			debugDepthShader.SetInt("far_plane", far_plane);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			renderQuad(quadVAO, quadVBO);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)windowWidth / (float)windowHeight, 0.1f, 3000.0f);
			glm::mat4 view = camera.GetViewMatrix();

			matrixUbo.SetData(0, &projection[0][0]);
			matrixUbo.SetData(1, &view[0][0]);

			lightSpaceMatrixUbo.SetData(0, &lightSpaceMatrix[0][0]);

			glActiveTexture(GL_TEXTURE0 + DEPTH_MAP_INDEX);
			glBindTexture(GL_TEXTURE_2D, depthMap);

			Shader::HOUSE->Bind();
			Shader::HOUSE->SetInt("u_EnableShadow", 1);
			Shader::HOUSE->SetInt("u_ShadowMap", DEPTH_MAP_INDEX);
			house.Draw(*Shader::HOUSE);
			wood.Draw();

			Shader::HEIGHTMAP->Bind();
			Shader::HEIGHTMAP->SetInt("u_EnableShadow", 1);
			Shader::HEIGHTMAP->SetInt("u_ShadowMap", DEPTH_MAP_INDEX);
			terrain.Draw();
			cubeMap.Draw();
			fire.Draw();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// bloom

			bool firstIteration = true;
			bool horizontal = true;
			shaderBlur.Bind();

			for (unsigned int i = 0; i < 10; i++)
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

		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteFramebuffers(1, &framebuffer);
	glDeleteFramebuffers(2, bloomFramebuffers);
	glDeleteTextures(2, sceneColorbuffers);
	glDeleteTextures(2, textureColorbuffers);
	glDeleteRenderbuffers(1, &rbo);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}