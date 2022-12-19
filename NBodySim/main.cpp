#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>

#ifndef GL_POINT_SPRITE
#  define GL_POINT_SPRITE 0x8861
#endif

static const struct
{
    float x, y;
    float r, g, b;
    float size;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f, 20.0f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f, 10.0f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f, 30.0f }
};

static const char* vertex_shader_text =
"#version 460\n"
"uniform mat4 vTransform;\n"
"uniform mat4 vModel;\n"
"uniform mat4 vView;\n"
"uniform mat4 vProjection;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"attribute float vSize;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = vProjection * vView * vModel * vTransform * vec4(vPos, 0.0, 1.0);\n"
"    gl_PointSize = vSize;\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 460\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    vec2 v = gl_PointCoord - vec2(0.5, 0.5);\n"
"    float r = v.x * v.x + v.y * v.y;\n"
"    if (r > 0.25) {\n"
"        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
"    } else {\n"
"        gl_FragColor = vec4(color, 1.0);\n"
"    }\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint vtransform_location, vmodel_location, vview_location, vprojection_location;
    GLint vpos_location, vcol_location, vsize_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
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

    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion =
        glGetString(GL_SHADING_LANGUAGE_VERSION);

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    printf("GL Version (string)  : %s\n", version);
    printf("GL Version (integer) : %d.%d\n", major, minor);
    printf("GLSL Version         : %s\n", glslVersion);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(vsize_location);
    glVertexAttribPointer(vsize_location, 1, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 5));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_POINT_SPRITE);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        float rps;
        float rotate_degree;
        int width, height;
        // mat4x4 m, p, mvp;
        glm::mat4 trans(1.0f);
        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        // mat4x4_identity(m);
        // mat4x4_rotate_Z(m, m, (float)glfwGetTime());
        // mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        // mat4x4_mul(mvp, p, m);

        // trans = glm::translate(trans, glm::vec3(0.0f, -0.2f, 0.0f));
        // trans = glm::rotate(trans, glm::radians(90.f), glm::vec3(0.0, 0.0, 1.0));
        // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

        rps = 0.1f;
        rotate_degree = fmod((float)glfwGetTime() * 360 * rps, 360);
        trans = glm::rotate(trans, glm::radians(rotate_degree), glm::vec3(0.0, 0.0, 1.0));

        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        projection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);

        glUseProgram(program);
        glUniformMatrix4fv(vtransform_location, 1, GL_FALSE, glm::value_ptr(trans));
        glUniformMatrix4fv(vmodel_location, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(vview_location, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(vprojection_location, 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_POINTS, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}