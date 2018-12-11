
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>
#include <GLES3/gl31.h>

#include "view.h"
#include "shaders.h"

static GLFWwindow* window;
static unsigned int spgs[4];
static unsigned int vaos[4];
static unsigned int vbos[4];

// points:
//   each point is six floats: x, y, r, g, b, a
// These are owned by the view thread:
static float* points = 0;
static unsigned int points_width = 6;
static unsigned int points_count = 0;

static pthread_t thread;
static pthread_mutex_t mutex;
// Mutex defends these two shared variables:
static float* next_points = 0;
static unsigned int next_count = 0;

int
view_set_points(float* data, int count)
{
    pthread_mutex_lock(&mutex);

    if (next_points) {
        free(next_points);
        next_points = 0;
    }

    next_points = data;
    next_count = count;

    pthread_mutex_unlock(&mutex);
    return 0;
}

static
void
on_glfw_error(int error, const char* description)
{
    puts("glfw error\r\n");
    puts(description);
}

static
void
on_framebuffer_resize(GLFWwindow* _window, int width, int height)
{
    printf("Resize: %d %d\n", width, height);
    glViewport(0, 0, width, height);
}

static
void
view_render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(spgs[0]);
    glBindVertexArray(vaos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glDrawArrays(GL_POINTS, 0, points_count);

    glfwSwapBuffers(window);
}

static
void*
view_thread_main(void* _arg)
{
    int ii;

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        pthread_mutex_lock(&mutex);
        if (next_points) {
            if (points) {
                free(points);
                points = 0;
            }

            points = next_points;
            points_count = next_count;

            next_points = 0;

            glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
            glBufferData(GL_ARRAY_BUFFER, points_count * 6 * sizeof(float),
                         points, GL_DYNAMIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                                  (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            printf("picked up %d new points\r\n", next_count);
        }
        pthread_mutex_unlock(&mutex);

        if (ii++ % 120 == 0) {
            printf("rendering %d points\r\n", points_count);
        }

        view_render();

        glfwPollEvents();
    }
}

int
view_init()
{
    int rv;
    pthread_mutex_init(&mutex, 0);

    glfwSetErrorCallback(on_glfw_error);
    rv = glfwInit();
    if (!rv) {
        return 0;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(800, 600, "gltest window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, on_framebuffer_resize);
    glViewport(0, 0, 800, 600);

    spgs[0] = compile_shader_program(
        "src/glsl/vertex.glsl",
        "src/glsl/fragment.glsl");

    glGenVertexArrays(4, vaos);
    glBindVertexArray(vaos[0]);

    glGenBuffers(4, vbos);

    rv = pthread_create(&thread, 0, view_thread_main, 0);
    return (rv == 0);
}

int
view_stop()
{
    pthread_mutex_lock(&mutex);
    glfwSetWindowShouldClose(window, 1);
    pthread_mutex_unlock(&mutex);

    int rv = pthread_join(thread, 0);
    glfwTerminate();

    if (next_points) {
        free(next_points);
        next_points = 0;
    }
    if (points) {
        free(points);
        points = 0;
    }

    return (rv == 0);
}

