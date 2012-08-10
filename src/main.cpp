#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>

#include <GL/GLee.h>
#include <GL/glfw3.h>

#include <joelang/context.hpp>
#include <joelang/effect.hpp>

bool running = true;
GLFWwindow window;

const JoeLang::Technique* t = nullptr;
JoeLang::Context* context = nullptr;

const std::string fragment_source =
    "#version 150\n"
    "out vec4 outputColor;\n"
    "void main()\n"
    "{\n"
    "   outputColor = vec4(0.44, 0.85, 0.29, 1.0);\n"
    "}\n";

GLuint position_buffer;
GLuint program;

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

GLuint CreateShader( GLenum shader_type, const std::string &shader_source )
{
    GLuint shader = glCreateShader(shader_type);
    const char* c = shader_source.c_str();
    glShaderSource( shader, 1, &c, NULL );

    glCompileShader(shader);

    GLint status;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
    if (status == GL_FALSE)
    {
        GLint info_log_length;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &info_log_length );

        GLchar* info_log = new GLchar[info_log_length + 1];
        glGetShaderInfoLog( shader, info_log_length, NULL, info_log );

        std::cerr << "Compile error in shader: " << info_log << std::endl;
        delete[] info_log;
    }

    return shader;
}

GLuint CreateProgram( const std::vector<GLuint> &shader_list )
{
    GLuint program = glCreateProgram();

    for( auto s : shader_list )
        glAttachShader( program, s );

    glLinkProgram( program );

    GLint status;
    glGetProgramiv( program, GL_LINK_STATUS, &status );
    if (status == GL_FALSE)
    {
        GLint info_log_length;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &info_log_length );

        GLchar *info_log = new GLchar[info_log_length + 1];
        glGetProgramInfoLog( program, info_log_length, NULL, info_log);

        std::cerr << "Link error in program: " << info_log << std::endl;
        delete[] info_log;
    }

    for( auto s : shader_list )
        glDetachShader( program, s );

    return program;
}

void InitializeProgram()
{
    std::vector<GLuint> shader_list;

    shader_list.push_back( CreateShader( GL_FRAGMENT_SHADER, fragment_source ) );

    program = CreateProgram( shader_list );

    std::for_each( shader_list.begin(), shader_list.end(), glDeleteShader );
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
    context = new JoeLang::Context();
    context->RegisterOpenGLStates();
    context->RegisterOpenGLActions();
    JoeLang::Effect* clear_blue =
                         context->CreateEffectFromFile( "data/clear_blue.jfx" );
    if( !clear_blue )
        return false;

    t = clear_blue->GetNamedTechnique( "clear_blue" );
    return t;
}

void ReleaseJoeLang()
{
    delete context;
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
    glBufferData( GL_ARRAY_BUFFER,
                  sizeof(vertex_positions),
                  vertex_positions,
                  GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    InitializeProgram();

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

    std::chrono::high_resolution_clock clock;

    auto old_time = clock.now();
    char title[64];

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

        auto new_time = clock.now();
        std::chrono::duration<float, std::milli> delta_time =
                                                            new_time - old_time;
        old_time = new_time;
        std::snprintf( title, 64, "JoeRender -- %.5fms", delta_time.count() );
        glfwSetWindowTitle( window, title );

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ReleaseJoeLang();

    // Terminate GLFW
    glfwTerminate();
}

