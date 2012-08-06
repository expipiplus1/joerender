#include <iostream>

#include <GL/glfw3.h>

bool running = true;

void OnKeyInput( GLFWwindow window, int k, int action )
{
  if( k == GLFW_KEY_ESCAPE &&
      action == GLFW_PRESS )
      running = false;
}

int OnWindowClosed(GLFWwindow window)
{
    running = false;
    return GL_TRUE;
}

int main()
{
    // Initialize glfw
    if( !glfwInit() )
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a window
    GLFWwindow window = glfwCreateWindow( 640, 480, GLFW_WINDOWED, "JoeRender", NULL );
    if (!window)
    {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "GL_RENDERER = " << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << std::endl;
    std::cout << "GL_VERSION  = " << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << std::endl;
    std::cout << "GL_VENDOR   = " << reinterpret_cast<const char*>(glGetString(GL_VENDOR)) << std::endl;

    glfwSetInputMode( window, GLFW_KEY_REPEAT, GL_TRUE );
    glfwSwapInterval( 1 );

    // Set callback functions
    glfwSetWindowCloseCallback( OnWindowClosed );
    glfwSetKeyCallback( OnKeyInput );

    // Main loop
    while( running )
    {
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();
}

