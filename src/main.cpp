#include <cassert>
#include <iostream>

#include <GL/GLee.h>
#include <GL/glfw3.h>

#include <joelang/context.hpp>
#include <joelang/effect.hpp>

bool running = true;
GLFWwindow window;
const JoeLang::Technique* t = nullptr;
JoeLang::Context context;
GLuint position_buffer;

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

bool InitializeGL()
{
    // Initialize glfw
    if( !glfwInit() )
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false; 
    }

#if defined(__APPLE__)
    glfwWindowHint( GLFW_OPENGL_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_OPENGL_VERSION_MINOR, 2 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#endif

    // Create a window
    window = glfwCreateWindow( 640, 480,
                               GLFW_WINDOWED,
                               "JoeRender",
                               NULL );
    if (!window)
    {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        return false;
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

    return true;
}

bool InitializeJoeLang()
{
    //
    // Initialize JoeLang
    //
    context.RegisterOpenGLStates();
    context.RegisterOpenGLActions();
    JoeLang::Effect* clear_blue =
                          context.CreateEffectFromFile( "data/clear_blue.jfx" );
    if( !clear_blue )
        return false;

    t = clear_blue->GetNamedTechnique( "clear_blue" );
    return t;
}

bool InitializeGLResources()
{
    const float vertex_positions[] = 
    {
        0.75f, 0.75f, 0.0f, 1.0f,
        0.75f, -0.75f, 0.0f, 1.0f,
        -0.75f, -0.75f, 0.0f, 1.0f,
    };

    glGenBuffers( 1, &position_buffer );
    glBindBuffer( GL_ARRAY_BUFFER, position_buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    return true;
}

int main()
{
    if( !InitializeGL() )
        return -1;

    if( !InitializeJoeLang() )
        return -1;

    if( !InitializeGLResources() )
        return -1;

    // Main loop
    while( running )
    {
        for( const JoeLang::Pass& pass : t->GetPasses() )
        {
            pass.SetState();

            glBindBuffer( GL_ARRAY_BUFFER, position_buffer );
            glEnableVertexAttribArray( 0 );
            glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, 0 );

            glDrawArrays( GL_TRIANGLES, 0, 3 );

            pass.ResetState();
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();
}

