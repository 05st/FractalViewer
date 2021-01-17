#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

unsigned int scr_width = 1280, scr_height = 720;
float pos_x = 0.0f, pos_y = 0.0f;
float zoom_level = 1.0f;

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
	scr_width = width;
	scr_height = height;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	zoom_level += ((yoffset > 0) ? 1 : -1) * 0.1f;
}

bool dragging = false;
double cstart_x = 0.0, cstart_y = 0.0;
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_1) {
		if (!dragging && action == GLFW_PRESS) {
			glfwGetCursorPos(window, &cstart_x, &cstart_y);
		}
		dragging = (action == GLFW_PRESS);
	}
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
	float x_change = 0.0f, y_change = 0.0f;
	if (dragging) {
		x_change += cstart_x - x;
		y_change += cstart_y - y;
	}
	pos_x += x_change / 320.0f / exp(zoom_level);
	pos_y -= y_change / 320.0f / exp(zoom_level);
	glfwGetCursorPos(window, &cstart_x, &cstart_y);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "Fractal Viewer", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create window";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, sizeCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD";
		return -1;
	}

	glViewport(0, 0, scr_width, scr_height);

	std::string vsSource = readFile("resources\\vertex.glsl");
	std::string fsSource = readFile("resources\\fragment.glsl");

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

	glfwSwapInterval(0);

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] = {
		 1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f,  1.0f
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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	int timeLoc = glGetUniformLocation(shaderProgram, "time");
	int zoomLevelLoc = glGetUniformLocation(shaderProgram, "zoomLevel");
	int windowSizeLoc = glGetUniformLocation(shaderProgram, "windowSize");
	int posLoc = glGetUniformLocation(shaderProgram, "pos");

	float pt = glfwGetTime();
	float timePassed = 0.0f;
	int frames = 0;

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float time = glfwGetTime();
		glUseProgram(shaderProgram);
		glUniform1f(timeLoc, time);
		glUniform1f(zoomLevelLoc, zoom_level);
		glUniform2f(windowSizeLoc, scr_width, scr_height);
		glUniform2f(posLoc, pos_x, pos_y);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		timePassed += (time - pt);
		if (timePassed >= 1.0f) {
			glfwSetWindowTitle(window, (std::string("Fractal Viewer | ") + std::to_string(frames)).c_str());
			timePassed = 0.0f;
			frames = 0;
		}
		frames++;
		pt = time;
	}

	glfwTerminate();
	return 0;
}
