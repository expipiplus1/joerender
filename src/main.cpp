#include <cassert>
#include <iostream>

#include <GL/glfw3.h>

#include <joelang/context.hpp>
#include <joelang/effect.hpp>

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

#if defined(__APPLE__)
    glfwWindowHint( GLFW_OPENGL_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_OPENGL_VERSION_MINOR, 2 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#endif

    // Create a window
    GLFWwindow window = glfwCreateWindow( 640, 480,
                                          GLFW_WINDOWED,
                                          "JoeRender",
                                          NULL );
    if (!window)
    {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "GL_RENDERER = " <<
                 reinterpret_cast<const char*>(glGetString(GL_RENDERER)) <<
                 std::endl;
    std::cout << "GL_VERSION  = " <<
                 reinterpret_cast<const char*>(glGetString(GL_VERSION)) <<
                 std::endl;
    std::cout << "GL_VENDOR   = " <<
                 reinterpret_cast<const char*>(glGetString(GL_VENDOR)) <<
                 std::endl;

    glfwSetInputMode( window, GLFW_KEY_REPEAT, GL_TRUE );
    glfwSwapInterval( 1 );

    // Set callback functions
    glfwSetWindowCloseCallback( OnWindowClosed );
    glfwSetKeyCallback( OnKeyInput );

    //
    // Initialize JoeLang
    //
    JoeLang::Context context;
    context.RegisterOpenGLStates();
    context.RegisterOpenGLActions();
    JoeLang::Effect* clear_blue =
                          context.CreateEffectFromFile( "data/clear_blue.jfx" );
    assert( clear_blue && "Couldn't get effect" );

    const JoeLang::Technique* t = clear_blue->GetNamedTechnique( "clear_blue" );
    assert( t );

    // Main loop
    while( running )
    {
        for( const JoeLang::Pass& pass : t->GetPasses() )
            pass.SetState();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();
}

