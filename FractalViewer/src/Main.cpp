#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <array>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>

const unsigned int WIDTH = 1280, HEIGHT = 720;
float x = 0.0f, y = 0.0f; // Panning
float zoom = 1.0f;
bool mandelbrotMode = true;
float j_re = 0.0f, j_im = 0.0f;
int max_iters = 1000;
bool cursor = true;

bool interpolate = false;
float re_s = 0.0f, re_e = 0.0f;
float im_s = 0.0f, im_e = 0.0f;
float v = 0.0f;

void init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

std::string readFile(std::string filePath) {
	std::ifstream ifs;
	ifs.open(filePath);

	std::stringstream stream;
	stream << ifs.rdbuf();
	ifs.close();

	return stream.str();
}

void sizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	double x_change = 0.0f, y_change = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		x_change += 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		x_change -= 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		y_change += 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		y_change -= 0.01f;
	}
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		x_change /= 10;
		y_change /= 10;
	}

	x += x_change / zoom;
	y += y_change / zoom;

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		zoom *= 1.005f;
	}
	else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		zoom *= 0.995f;
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		mandelbrotMode = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		mandelbrotMode = false;
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		cursor = !cursor;
	}
}

int main() {
	init();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fractal Viewer", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, sizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	std::string vsSource = readFile("src\\vertex.glsl");
	std::string fsSource = readFile("src\\fragment.glsl");

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexcstr = vsSource.c_str();
	glShaderSource(vertexShader, 1, &vertexcstr, nullptr);
	glCompileShader(vertexShader);

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentcstr = fsSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentcstr, nullptr);
	glCompileShader(fragmentShader);

	int success;
	char infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Error compiling fragment shader: " << infoLog << std::endl;
	}

	glfwSwapInterval(1);

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] = {
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	unsigned int ebo;
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	std::cout << "Fractal Viewer" << std::endl << std::endl;
	std::cout << "Controls" << std::endl;
	std::cout << "[Arrow Keys] Movement" << std::endl;
	std::cout << "[Left Shift] Precise Movement" << std::endl;
	std::cout << "[Z] Zoom In" << std::endl;
	std::cout << "[X] Zoom Out" << std::endl;
	std::cout << "[M] Mandelbrot Set" << std::endl;
	std::cout << "[J] Julia Set" << std::endl;
	std::cout << "[C] Toggle Cursor" << std::endl;
	std::cout << std::endl;
	std::cout << "Console Commands" << std::endl;
	std::cout << "[1] Set Cursor Position" << std::endl;
	std::cout << "[2] Set Zoom" << std::endl;
	std::cout << "[3] Change Julia Set Constant" << std::endl;
	std::cout << "[4] Interpolate Julia Set Constant" << std::endl;
	std::cout << "[5] Change Max Iterations" << std::endl;
	std::cout << std::endl;

	bool checkInput = true;

	std::thread consoleThread([&]() {
		while (checkInput) {
			int input;
			std::cin >> input;

			if (input == 1) {
				float n_x = 0.0f, n_y = 0.0f;
				std::cout << "Real: ";
				std::cin >> n_x;
				std::cout << "Imag: ";
				std::cin >> n_y;
				x = n_x;
				y = n_y;
			}
			else if (input == 2) {
				float n_z = 0.0f;
				std::cout << "Zoom: ";
				std::cin >> n_z;
				zoom = n_z;
			}
			else if (input == 3) {
				float n_re = 0.0f, n_im = 0.0f;
				std::cout << "Real: ";
				std::cin >> n_re;
				std::cout << "Imag: ";
				std::cin >> n_im;
				j_re = n_re;
				j_im = n_im;
			}
			else if (input == 4) {
				std::cout << "Real Start: ";
				std::cin >> re_s;
				std::cout << "Real End: ";
				std::cin >> re_e;
				std::cout << "Imag Start: ";
				std::cin >> im_s;
				std::cout << "Imag End: ";
				std::cin >> im_e;
				interpolate = true;
			}
			else if (input == 5) {
				int n_iters = 0;
				std::cout << "Max Iters: ";
				std::cin >> n_iters;
				max_iters = n_iters;
			}
		}
	});

	int zoomLocation = glGetUniformLocation(shaderProgram, "zoom");
	int timeLocation = glGetUniformLocation(shaderProgram, "time");
	int posLocation = glGetUniformLocation(shaderProgram, "pos");
	int windowSizeLocation = glGetUniformLocation(shaderProgram, "windowSize");
	int mandelbrotModeLocation = glGetUniformLocation(shaderProgram, "mandelbrotMode");
	int jcLocation = glGetUniformLocation(shaderProgram, "j_c");
	int maxItersLocation = glGetUniformLocation(shaderProgram, "max_iters");
	int cursorLocation = glGetUniformLocation(shaderProgram, "cursor");

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (interpolate) {
			if (v <= 1.0f) {
				j_re = (re_s * (1.0f - v)) + (re_e * v);
				j_im = (im_s * (1.0f - v)) + (im_e * v);
				v += 0.005f;
			}
			else {
				j_re = re_e;
				j_im = im_e;
				re_s = 0.0f;
				im_s = 0.0f;
				re_e = 0.0f;
				im_e = 0.0f;
				interpolate = false;
				v = 0.0f;
			}
		}

		float time = glfwGetTime();
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glUseProgram(shaderProgram);
		glUniform1f(zoomLocation, zoom);
		glUniform1f(timeLocation, time);
		glUniform2f(posLocation, x, y);
		glUniform2f(windowSizeLocation, width, height);
		glUniform1i(mandelbrotModeLocation, (int)mandelbrotMode);
		glUniform2f(jcLocation, j_re, j_im);
		glUniform1i(maxItersLocation, max_iters);
		glUniform1i(cursorLocation, (int)cursor);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		std::string posString = "(" + std::to_string(x) + " + " + std::to_string(y) + "i)";
		std::string zoomString = " [" + std::to_string(zoom) + "x]";

		glfwSetWindowTitle(window, (std::string("Fractal Viewer ") + posString + zoomString).c_str());
	}

	checkInput = false;
	glfwTerminate();
	std::exit(0);
	return 0;
}
