#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include <stdio.h>
#include "Application.h"
#include "freeimage.h"

void PrintOpenGLVersion();

int main()
{
    FreeImage_Initialise();
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    // Initial Sizes for the Window Size
    int WindowWidth = 1200;
    int WindowHeight = 800;

    // Primary Monitor and it's Videomode to get the specific Width and Height
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

    //WindowWidth = videoMode->width;
    //WindowHeight = videoMode->height;

    GLFWwindow *window = glfwCreateWindow(WindowWidth, WindowHeight, "CG_Projekt - Kirkesler, Knodt", NULL, NULL);

    if (!window){
        fprintf(stderr, "ERROR: can not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

#if WIN32
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    PrintOpenGLVersion();

    {
        double lastTime = 0;
        Application App(window, WindowWidth, WindowHeight);
        App.start();
        while (!glfwWindowShouldClose(window)) {
            // Nur fuer das Testen, um per ESC aus dem Fullscreen rauszukommen, nicht funktionsfaehig
            int width = 1200;
            int height = 800;
            if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                glfwSetWindowMonitor(window, NULL, 0, 0, width, height, GLFW_DONT_CARE);
                glfwGetFramebufferSize(window, &width, &height);
                glViewport(0, 0, width, height);
            }
            if (glfwGetKey(window, GLFW_KEY_F)) {
                glfwSetWindowMonitor(window, monitor, 0, 0, WindowWidth, WindowHeight, GLFW_DONT_CARE);
                glfwGetFramebufferSize(window, &width, &height);
                glViewport(0, 0, width, height);
            }

            double now = glfwGetTime();
            double delta = now - lastTime;
            lastTime = now;

            // once per frame
            glfwPollEvents();
            App.update((float)delta);
            App.draw();
            App.showText();
            glfwSwapBuffers(window);
        }
        App.end();
    }

    glfwTerminate();
    return 0;
}

void PrintOpenGLVersion()
{
    // get version info
    const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte *version = glGetString(GL_VERSION);   // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
}
