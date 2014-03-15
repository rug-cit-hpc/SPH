#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "glfw_utils.h"
#include "renderer.h"

void check_key_press(GL_STATE_T *state)
{
    // Poll GLFW for key press
    // If key has been pressed key_callback should be called
    glfwPollEvents();
}

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}


bool window_should_close(GL_STATE_T *state)
{
    if(glfwWindowShouldClose(state->window))
	return true;
    else
	return false;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Get render_state from GLFW user pointer
    RENDER_T *render_state = glfwGetWindowUserPointer(window);

    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch(key)
        { 
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
	        break;
            case GLFW_KEY_RIGHT:
                increase_parameter(render_state);
                break;
            case GLFW_KEY_LEFT:
                decrease_parameter(render_state);
                break;
            case GLFW_KEY_UP:
                move_parameter_up(render_state);
                break;
            case GLFW_KEY_DOWN:
                move_parameter_down(render_state);
                break;
            case GLFW_KEY_LEFT_BRACKET:
                remove_partition(render_state);
                break;
            case GLFW_KEY_RIGHT_BRACKET:
                add_partition(render_state);
                break;
            case GLFW_KEY_X:
                set_fluid_x(render_state);
                break;
            case GLFW_KEY_Y:
                set_fluid_y(render_state);
                break;
            case GLFW_KEY_A:
                set_fluid_a(render_state);
                break;
            case GLFW_KEY_B:
                set_fluid_b(render_state);
                break;
        }
    }
}

// Return mouse position in OpenGL screen coordinates
// x,y [-1, 1], center of screen is origin
void get_mouse(float *x, float *y, GL_STATE_T *state)
{
    double mx, my;
    glfwGetCursorPos(state->window, &mx, &my);
    *y = (state->screen_height - my); // Flip y = 0
    *y = *y/(0.5*state->screen_height) - 1.0;
    *x = mx/(0.5*state->screen_width) - 1.0;
}

// scroll wheel callback
void wheel_callback(GLFWwindow* window, double x, double y)
{
    // Get render_state from GLFW user pointer
    RENDER_T *render_state = glfwGetWindowUserPointer(window);
    
    // Call increase/decrease mover calls
    if(y > 0.0)
	increase_mover_height(render_state);
    else if(y < 0.0)
	decrease_mover_height(render_state);
    if(x > 0.0)
        increase_mover_width(render_state);
    else if(x < 0.0)
        decrease_mover_width(render_state);
}

// Description: Sets the display, OpenGL context and screen stuff
void init_ogl(GL_STATE_T *state, RENDER_T *render_state)
{
    // Set error callback
    glfwSetErrorCallback(error_callback);

    // Initialize GLFW
    if(!glfwInit())
        exit(EXIT_FAILURE);

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Try to get full screen
    // Retina screen is a pain...
    int width, height;
    width = 1920;
    height = 1080;
    state->window = glfwCreateWindow(width, height, "SPH", glfwGetPrimaryMonitor(), NULL);

    glfwGetFramebufferSize(state->window, &state->screen_width, &state->screen_height);
    glViewport(0, 0, state->screen_width, state->screen_height);

    if(!state->window)
	exit(EXIT_FAILURE);

    // Set current context to window
    glfwMakeContextCurrent(state->window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // Set key callback
    glfwSetKeyCallback(state->window, key_callback);

    // Set scroll wheel callback
    glfwSetScrollCallback(state->window, wheel_callback);

    // Add render state to window pointer
    // Used for key callbacks
    glfwSetWindowUserPointer(state->window, render_state);

    // Disable vsync for true FPS testing
    // Default limit 60 fps
//    glfwSwapInterval(0);

    // Set background color and clear buffers
    glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT );
}

void swap_ogl(GL_STATE_T *state)
{
    glfwSwapBuffers(state->window);

    glfwPollEvents();
}

void exit_ogl(GL_STATE_T *state)
{
//    glDeleteProgram(state->shaderProgram);
//    glDeleteShader(fragmentShader);
//    glDeleteShader(vertexShader);
//    glDeleteBuffers(1, &vbo);
//    glDeleteVertexArrays(1, &vao);

    glfwDestroyWindow(state->window);
    glfwTerminate();

    printf("close\n");
}
