#define _CRT_SECURE_NO_WARNINGS
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#include <glm/glm.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "opengl_context.h"

#include "option.h"
#include "point.h"
#include "nbody.h"
#include "camera.h"

__declspec(align(0x10)) static point vertices_1[POINT_CNT];
__declspec(align(0x10)) static point vertices_2[POINT_CNT];

static void light()
{
	GLfloat light_specular[] = { 0.4, 0.4, 0.4, 1 };
	GLfloat light_diffuse[] = { 0.8,0.8,0.8, 1 };
	GLfloat light_ambient[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat light_position[] = { 10, 10, 10, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

static void errorCallback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void resizeCallback(GLFWwindow *window, int width, int height)
{
	OpenGLContext::framebufferResizeCallback(window, width, height);
	auto ptr = static_cast<Camera *>(glfwGetWindowUserPointer(window));

	if (ptr) {
		ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
	}
}

static void keyCallback(GLFWwindow *window, int key, int, int action, int)
{
	if (action == GLFW_REPEAT)
		return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void initOpenGL()
{
	OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
	GLFWwindow *window = OpenGLContext::getWindow();

	glfwSetWindowTitle(window, "NBodySim");
	glfwSetErrorCallback(errorCallback);
	glfwSetFramebufferSizeCallback(window, resizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

static void showVersion(void)
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

static void initPoints(void)
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

static void banner(void)
{
	printf("======== NBODYSIM ========\n");
	printf("1: Serial\n");
	printf("2: Serial SIMD\n");
	printf("3: Parallel SIMD\n");
	printf("> ");
}

void drawSphere(double r, int lats, int longs, float px, float py, float pz)
{
	int i, j;

	for (i = 0; i <= lats; i++) {
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++) {
			double lng = 2 * M_PI * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(r * x * zr0 + px, r * y * zr0 + py + 4000, r * z0 + pz);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(r * x * zr1 + px, r * y * zr1 + py + 4000, r * z1 + pz);
		}
		glEnd();
	}
}

int main(void)
{
	GLFWwindow *window;
	double prevtime;
	point *points1 = vertices_1, *points2 = vertices_2;
	unsigned long long int estimate_round = 0;
	double estimate_time = 0, estimate_prevtime;
	nBodyFunc calculateNBody;
	int choice;

	initOpenGL();

	window = OpenGLContext::getWindow();

	// Init Camera helper
	Camera camera(glm::vec3(0, 2, 5));
	camera.initialize(OpenGLContext::getAspectRatio());

	// Store camera as glfw global variable for callbasks use
	glfwSetWindowUserPointer(window, &camera);

	showVersion();

	initPoints();

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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

#ifndef DISABLE_LIGHT
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	light();
#endif

	while (!glfwWindowShouldClose(window)) {
		double dt; // delta time
		double currtime;
		point *tmp;

		// Polling events.
		glfwPollEvents();

		// Update camera position and view
		camera.move(window);

		// GL_XXX_BIT can simply "OR" together to use.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(camera.getProjectionMatrix());

		// ModelView Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(camera.getViewMatrix());

#ifndef DISABLE_LIGHT
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
#endif

		currtime = (float)glfwGetTime();
		dt = currtime - prevtime;
		prevtime = currtime;

		estimate_round += 1;
		estimate_prevtime = glfwGetTime();

		calculateNBody(points1, points2, dt * DELTA_TIME_MUL);

		estimate_time += glfwGetTime() - estimate_prevtime;

		tmp = points1;
		points1 = points2;
		points2 = tmp;

		glScalef(0.0001f, 0.0001f, 0.0001f);

		for (int i = 0; i < POINT_CNT; i++) {
			glColor3f(points1[i]._r, points1[i]._g, points1[i]._b);
			drawSphere(points1[i]._mass, 10, 10, points1[i]._x, points1[i]._y, points1[i]._z);
		}

		// Update FPS
		if (dt != 0 && estimate_round % 10 == 0) {
			std::string title = "FPS = " + std::to_string((int)(1 / dt));

			glfwSetWindowTitle(window, title.c_str());
		}

		glfwSwapBuffers(window);
	}

	printf("Total round: %lld\n", estimate_round);
	printf("Avg calculating time: %f ms\n", estimate_time * 1000 / estimate_round);

	exit(EXIT_SUCCESS);
}