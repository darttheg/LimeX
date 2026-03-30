#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>

static bool glfw_get_window_monitor(
    GLFWmonitor** monitor,
    GLFWwindow* window
) {
    bool success = false;

    int window_rectangle[4] = { 0 };
    glfwGetWindowPos(window, &window_rectangle[0], &window_rectangle[1]);
    glfwGetWindowSize(window, &window_rectangle[2], &window_rectangle[3]);

    int monitors_size = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitors_size);

    GLFWmonitor* closest_monitor = NULL;
    int max_overlap_area = 0;

    for (int i = 0; i < monitors_size; ++i)
    {
        int monitor_rectangle[4] = { 0 }; // <-- only position was computed, not rectangle
        glfwGetMonitorWorkarea(monitors[i], &monitor_rectangle[0], &monitor_rectangle[1], &monitor_rectangle[2], &monitor_rectangle[3]);

        const GLFWvidmode* monitor_video_mode = glfwGetVideoMode(monitors[i]);

        if (
            !(
                ((window_rectangle[0] + window_rectangle[2]) < monitor_rectangle[0]) ||
                (window_rectangle[0] > (monitor_rectangle[0] + monitor_rectangle[2])) ||
                ((window_rectangle[1] + window_rectangle[3]) < monitor_rectangle[1]) ||
                (window_rectangle[1] > (monitor_rectangle[1] + monitor_rectangle[3]))
                )
            ) {
            int intersection_rectangle[4] = { 0 };

            // x, width
            if (window_rectangle[0] < monitor_rectangle[0])
            {
                intersection_rectangle[0] = monitor_rectangle[0];

                if ((window_rectangle[0] + window_rectangle[2]) < (monitor_rectangle[0] + monitor_rectangle[2]))
                {
                    intersection_rectangle[2] = (window_rectangle[0] + window_rectangle[2]) - intersection_rectangle[0];
                }
                else
                {
                    intersection_rectangle[2] = monitor_rectangle[2];
                }
            }
            else
            {
                intersection_rectangle[0] = window_rectangle[0];

                if ((monitor_rectangle[0] + monitor_rectangle[2]) < (window_rectangle[0] + window_rectangle[2]))
                {
                    intersection_rectangle[2] = (monitor_rectangle[0] + monitor_rectangle[2]) - intersection_rectangle[0];
                }
                else
                {
                    intersection_rectangle[2] = window_rectangle[2];
                }
            }

            // y, height
            if (window_rectangle[1] < monitor_rectangle[1])
            {
                intersection_rectangle[1] = monitor_rectangle[1];

                if ((window_rectangle[1] + window_rectangle[3]) < (monitor_rectangle[1] + monitor_rectangle[3]))
                {
                    intersection_rectangle[3] = (window_rectangle[1] + window_rectangle[3]) - intersection_rectangle[1];
                }
                else
                {
                    intersection_rectangle[3] = monitor_rectangle[3];
                }
            }
            else
            {
                intersection_rectangle[1] = window_rectangle[1];

                if ((monitor_rectangle[1] + monitor_rectangle[3]) < (window_rectangle[1] + window_rectangle[3]))
                {
                    intersection_rectangle[3] = (monitor_rectangle[1] + monitor_rectangle[3]) - intersection_rectangle[1];
                }
                else
                {
                    intersection_rectangle[3] = window_rectangle[3];
                }
            }

            int overlap_area = intersection_rectangle[2] * intersection_rectangle[3];
            if (overlap_area > max_overlap_area)
            {
                closest_monitor = monitors[i];
                max_overlap_area = overlap_area;
            }
        }
    }

    if (closest_monitor)
    {
        *monitor = closest_monitor;
        success = true;
    }

    // true: monitor contains the monitor the window is most on
    // false: monitor is unmodified
    return success;
}