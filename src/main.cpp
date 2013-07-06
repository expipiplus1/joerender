#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>

#include <GL/GLee.h>
//#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <joelang/context.hpp>
#include <joelang/effect.hpp>
#include <joelang/parameter.hpp>

#include <joemath/joemath.hpp>

bool running = true;
GLFWwindow* window;

JoeLang::Context* context = nullptr;
JoeLang::Effect* effect = nullptr;
const JoeLang::Technique* technique = nullptr;

GLuint triangle_vertex_array_object = 0;
GLuint triangle_position_buffer = 0;
GLuint fullscreen_vertex_array_object = 0;
GLuint index_buffer = 0;

bool render_triangle = false;
bool render_fullscreen = false;

void OnKeyInput( GLFWwindow* window, int k, int action, int mods )
{
    if( action == GLFW_PRESS && ( k == GLFW_KEY_ESCAPE || k == 'Q' ) )
        running = false;
}

void OnWindowClosed( GLFWwindow* window )
{
    running = false;
}

void OnWindowResize( GLFWwindow* window, int width, int height )
{
    //JoeLang::Parameter<JoeMath::int2>* window_size = effect->GetNamedParameter<JoeMath::int2>(
        //"window_size" );
    //assert( window_size );
    //window_size->SetParameter( JoeMath::int2( width, height ) );

    glViewport( 0, 0, width, height );
}

bool InitializeGL()
{
    // Initialize glfw
    if( !glfwInit() )
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

#if defined( __APPLE__ )
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#endif

    // Create a window
    window = glfwCreateWindow( 640, 480, "JoeRender", nullptr, nullptr );
    if( !window )
    {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        return false;
    }

    //glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_FALSE );
    glfwMakeContextCurrent( window );
    //glfwSwapInterval( 1 );

    // Set callback functions
    glfwSetWindowCloseCallback( window, OnWindowClosed );
    glfwSetWindowSizeCallback( window, OnWindowResize );
    glfwSetKeyCallback( window, OnKeyInput );

    glViewport( 0, 0, 640, 480 );

    //std::cout << "GL_RENDERER = " << reinterpret_cast<const char*>( glGetString( GL_RENDERER ) )
              //<< std::endl;
    //std::cout << "GL_VERSION  = " << reinterpret_cast<const char*>( glGetString( GL_VERSION ) )
              //<< std::endl;
    //std::cout << "GL_VENDOR   = " << reinterpret_cast<const char*>( glGetString( GL_VENDOR ) )
              //<< std::endl;

    return true;
}

bool InitializeJoeLang()
{
    //
    // Initialize JoeLang
    //
    context = new JoeLang::Context();
    context->SetErrorCallback([]( std::string e ) {
        std::cerr << e << std::endl;
    } );
    context->RegisterOpenGLStates();
    context->RegisterOpenGLActions();

    //
    // The state used to render our scene
    //
    static JoeLang::State<bool> render_triangle_state( "render_triangle" );
    render_triangle_state.SetCallbacks([]( bool b ) {
        render_triangle = b;
    },
                                       []() {
        render_triangle = false;
    } );
    context->AddState( &render_triangle_state );

    static JoeLang::State<bool> render_fullscreen_state( "render_fullscreen" );
    render_fullscreen_state.SetCallbacks([]( bool b ) {
        render_fullscreen = b;
    },
                                         []() {
        render_fullscreen = false;
    } );
    context->AddState( &render_fullscreen_state );

    static JoeLang::State<float> float_state( "float_state" );
    float_state.SetCallbacks([]( float f ) {
        std::cout << "float_state: " << f << std::endl;
    },
                             []() {
        std::cout << "float_state reset" << std::endl;
    } );
    context->AddState( &float_state );

    static JoeLang::State<JoeMath::float4x4> matrix_state( "matrix_state" );
    matrix_state.SetCallbacks([]( JoeMath::float4x4 m ) {
        std::cout << "matrix_state: " << std::endl 
                  << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << " " 
                  << std::endl 
                  << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << " " 
                  << std::endl 
                  << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << " " 
                  << std::endl 
                  << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << " " 
                  << std::endl;
    },
                              []() {
        std::cout << "matrix_state reset" << std::endl;
    } );
    context->AddState( &matrix_state );


    effect = context->CreateEffectFromFile( "data/clear_blue.jfx" );
    if( !effect )
        return false;

    technique = effect->GetNamedTechnique( "clear_blue" );
    return technique;
}

bool InitializeGLResources()
{
    const unsigned vertex_indices[] = { 0, 1, 2, };

    glGenBuffers( 1, &index_buffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                  sizeof( vertex_indices ),
                  vertex_indices,
                  GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


    const float triangle_vertex_positions[] = { 0.75f, 0.75f, 0.0f, 1.0f, 0.75f, -0.75f, 0.0f, 1.0f,
                                                -0.75f, -0.75f, 0.0f, 1.0f, };

    glGenBuffers( 1, &triangle_position_buffer );
    glBindBuffer( GL_ARRAY_BUFFER, triangle_position_buffer );

    glBufferData( GL_ARRAY_BUFFER,
                  sizeof( triangle_vertex_positions ),
                  triangle_vertex_positions,
                  GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );


    glGenVertexArrays( 1, &triangle_vertex_array_object );
    glBindVertexArray( triangle_vertex_array_object );

    glBindBuffer( GL_ARRAY_BUFFER, triangle_position_buffer );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );

    //
    // We don't need to unbind GL_ELEMENT_ARRAY_BUFFER because the state is
    // contained in the vao now.
    //

    glBindVertexArray( 0 );

    glGenVertexArrays( 1, &fullscreen_vertex_array_object );
    glBindVertexArray( fullscreen_vertex_array_object );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
    //
    // We don't need to unbind GL_ELEMENT_ARRAY_BUFFER because the state is
    // contained in the vao now.
    //

    glBindVertexArray( 0 );

    return true;
}

int main()
{
    if( !InitializeGL() )
        return 1;

    if( !InitializeJoeLang() )
        return 2;

    if( !InitializeGLResources() )
        return 3;

    /*
    JoeLang::Parameter<JoeMath::float3>* red = effect->GetNamedParameter<JoeMath::float3>( "red" );
    if( !red )
        return 4;
    red->SetParameter( JoeMath::float3( 0.85, 0.29, 0.29 ) );

    JoeLang::Parameter<JoeMath::float3x3>* rotate = effect->GetNamedParameter<JoeMath::float3x3>(
        "rotate" );
    if( !rotate )
        return 4;

    JoeLang::Parameter<JoeMath::float3x3>* mirror = effect->GetNamedParameter<JoeMath::float3x3>(
        "mirror" );
    if( !mirror )
        return 4;
    mirror->SetParameter( JoeMath::float3x3( -1, 0, 0, 0, 1, 0, 0, 0, 1 ) );

    JoeLang::Parameter<JoeMath::int2>* window_size = effect->GetNamedParameter<JoeMath::int2>(
        "window_size" );
    if( !window_size )
        return 4;
    window_size->SetParameter( { 640, 480 } );
    */

    std::chrono::high_resolution_clock clock;
    unsigned long long num_frames = 0;
    auto start_time = clock.now();

    auto old_time = clock.now();
    char title[64];
    
    // Main loop
    while( running )
    {
        //std::chrono::duration<float> seconds = std::chrono::steady_clock::now().time_since_epoch();
        //rotate->SetParameter( JoeMath::Rotate2D( seconds.count() ) );

        for( const JoeLang::Pass& pass : technique->GetPasses() )
        {
            pass.SetState();

            if( render_triangle )
            {
                //glBindBuffer( GL_ARRAY_BUFFER, triangle_position_buffer );
                glBindVertexArray( triangle_vertex_array_object );
                glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0 );
                glBindVertexArray( 0 );
                glBindBuffer( GL_ARRAY_BUFFER, 0 );
            }
            if( render_fullscreen )
            {
                glBindVertexArray( fullscreen_vertex_array_object );
                glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0 );
                glBindVertexArray( 0 );
            }

            pass.ResetState();
        }

        auto new_time = clock.now();
        std::chrono::duration<float, std::milli> delta_time = new_time - old_time;
        old_time = new_time;
        std::snprintf( title, 64, "JoeRender -- %.5fms", delta_time.count() );
        glfwSetWindowTitle( window, title );

        // Swap buffers
        glfwSwapBuffers( window );
        glfwPollEvents();
        ++num_frames;
    }

    auto new_time = clock.now();
    std::chrono::duration<float> delta_time = new_time - start_time;

    std::cout << num_frames << " frames in " << delta_time.count() << "s ("
              << 1000 * delta_time.count() / num_frames << "ms)\n";

    delete context;
    
    glfwTerminate();
    
    return 1;    
}
