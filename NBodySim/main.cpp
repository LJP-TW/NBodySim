#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>

#include "option.h"
#include "point.h"
#include "nbody.h"

#ifndef GL_POINT_SPRITE
#  define GL_POINT_SPRITE 0x8861
#endif

static point vertices_1[POINT_CNT];
static point vertices_2[POINT_CNT];

static const char *vertex_shader_text =
"#version 460\n"
"uniform mat4 vTransform;\n"
"uniform mat4 vModel;\n"
"uniform mat4 vView;\n"
"uniform mat4 vProjection;\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"attribute float vSize;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = vProjection * vView * vModel * vTransform * vec4(vPos, 1.0);\n"
"    gl_PointSize = vSize;\n"
"    color = vCol;\n"
"}\n";

static const char *fragment_shader_text =
"#version 460\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    vec2 v = gl_PointCoord - vec2(0.5, 0.5);\n"
"    float r = v.x * v.x + v.y * v.y;\n"
"    if (r >= 0.2) {\n"
"        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
"    } else {\n"
"        gl_FragColor = vec4(color, 1.0);\n"
"    }\n"
"}\n";

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void show_version(void)
{
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion =
		glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Version (string)  : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version         : %s\n", glslVersion);
}

void init_points(void)
{
	for (int i = 0; i < POINT_CNT; ++i) {
#ifdef SEED
		unsigned int seed = SEED;
#else
#	ifdef _DEBUG
		unsigned int seed = 5566888 - 100 * i;
#	else
		unsigned int seed = time(NULL) - 100 * i;
#	endif
#endif

		vertices_1[i] = point(seed);
		vertices_2[i] = vertices_1[i];
	}
}

void banner(void)
{
	printf("======== NBODYSIM ========\n");
	printf("1: Serial\n");
	printf("2: Serial SIMD\n");
	printf("3: Parallel SIMD\n");
	printf("> ");
}

int main(void)
{
	GLFWwindow *window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint vtransform_location, vmodel_location, vview_location, vprojection_location;
	GLint vpos_location, vcol_location, vsize_location;
	double prevtime;
	point *points1 = vertices_1, *points2 = vertices_2;
	unsigned long long int estimate_round = 0;
	double estimate_time = 0, estimate_prevtime;
	nBodyFunc calculateNBody;
	int choice;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1280, 720, "NBodySim", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSwapInterval(1);

	// NOTE: OpenGL error checks have been omitted for brevity

	show_version();

	init_points();

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	vtransform_location = glGetUniformLocation(program, "vTransform");
	vmodel_location = glGetUniformLocation(program, "vModel");
	vview_location = glGetUniformLocation(program, "vView");
	vprojection_location = glGetUniformLocation(program, "vProjection");
	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");
	vsize_location = glGetAttribLocation(program, "vSize");

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertices_1[0]), (void *)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertices_1[0]), (void *)(sizeof(float) * 4));
	glEnableVertexAttribArray(vsize_location);
	glVertexAttribPointer(vsize_location, 1, GL_FLOAT, GL_FALSE,
		sizeof(vertices_1[0]), (void *)(sizeof(float) * 7));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_PROGRAM_POINT_SIZE);

	glEnable(GL_POINT_SPRITE);
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

	banner();
	scanf("%d", &choice);
	switch (choice) {
	case 1:
		calculateNBody = nBodyCalculateSerial;
		break;
	case 2:
		calculateNBody = nBodyCalculateSerialSIMD;
		break;
	case 3:
		calculateNBody = nBodyCalculateParallelSIMD;
		break;
	default:
		calculateNBody = nBodyCalculateSerial;
	}

	prevtime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		double dt; // delta time
		double currtime;
		point *tmp;
		// mat4x4 m, p, mvp;
		glm::mat4 trans(1.0f);
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 projection(1.0f);

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		currtime = (float)glfwGetTime();
		dt = currtime - prevtime;
		prevtime = currtime;

		dt *= DELTA_TIME_MUL;

		estimate_round += 1;
		estimate_prevtime = glfwGetTime();

		calculateNBody(points1, points2, dt);

		estimate_time += glfwGetTime() - estimate_prevtime;

		tmp = points1;
		points1 = points2;
		points2 = tmp;

		// Update buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_1), points1);

		// Update matrices
		// mapping (10000.0, 10000.0) to (1.0, 1.0)
		model = glm::scale(model, glm::vec3(0.0001f, 0.0001f, 0.0001f));

		// TODO: Consider z-axis
		// view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
		// projection = glm::perspective(glm::radians(110.0f), ratio, 0.5f, 100.0f);

		glUseProgram(program);
		glUniformMatrix4fv(vtransform_location, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(vmodel_location, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(vview_location, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(vprojection_location, 1, GL_FALSE, glm::value_ptr(projection));
		glDrawArrays(GL_POINTS, 0, POINT_CNT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	printf("Total round: %lld\n", estimate_round);
	printf("Avg calculating time: %f ms\n", estimate_time * 1000 / estimate_round);

	exit(EXIT_SUCCESS);
}