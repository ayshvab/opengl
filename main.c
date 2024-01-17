#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>

#define GL_LOG_FILE "gl.log"

typedef int32_t b32;

#define ARRAY_SIZE( arr ) ( sizeof( arr ) / sizeof( ( arr )[0] ) )
#define handle_error()                                                                                                                                                                                 \
  ( {                                                                                                                                                                                                  \
    printf( "Error %s\n", strerror( errno ) );                                                                                                                                                         \
    exit( -1 );                                                                                                                                                                                        \
  } )


b32 restart_gl_log() {
  FILE* file = fopen( GL_LOG_FILE, "w" );
  if ( !file ) {
    fprintf( stderr, "ERROR: could not open GL_LOG_FILE log file %s for writing\n", GL_LOG_FILE );
    return 0;
  }
  time_t now = time( NULL );
  char* date = ctime( &now );
  fprintf( file, "GL_LOG_FILE log, local time %s\n", date );
  fclose( file );
  return 1;
}

b32 gl_log( const char* message, ... ) {
  va_list argptr;

  FILE* file = fopen( GL_LOG_FILE, "a" );
  if ( !file ) {
    fprintf( stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", GL_LOG_FILE );
    return 0;
  }

  va_start( argptr, message );
  vfprintf( file, message, argptr );
  va_end( argptr );
  fclose( file );
  return 1;
}

b32 gl_log_err( const char* message, ... ) {
  va_list argptr;

  FILE* file = fopen( GL_LOG_FILE, "a" );
  if ( !file ) {
    fprintf( stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", GL_LOG_FILE );
    return 0;
  }

  va_start( argptr, message );
  vfprintf( file, message, argptr );
  va_end( argptr );

  va_start( argptr, message );
  vfprintf( stderr, message, argptr );
  va_end( argptr );

  fclose( file );
  return 1;
}

char* read_shader( char* filepath ) {
  FILE* fp = fopen( filepath, "r" );
  if ( !fp ) { handle_error(); }

  size_t buffer_size = 4096;
  char* buffer       = calloc( buffer_size, 1 );
  fread( buffer, buffer_size, sizeof( *buffer ), fp );

  return buffer;
}

void glfw_error_callback(int error, const char* description) {
  gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

void log_gl_params() {
  GLenum params[] = {
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_DRAW_BUFFERS,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VARYING_FLOATS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS,
    GL_MAX_VIEWPORT_DIMS,
    GL_STEREO,
  };
  const char* names[] = {
    "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
    "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
    "GL_MAX_DRAW_BUFFERS",
    "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
    "GL_MAX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_TEXTURE_SIZE",
    "GL_MAX_VARYING_FLOATS",
    "GL_MAX_VERTEX_ATTRIBS",
    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
    "GL_MAX_VIEWPORT_DIMS",
    "GL_STEREO",    
  };
  gl_log("GL Context Params:\n");
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv(params[i], &v);
    gl_log("%s %i\n", names[i], v);
  }
  // others
  int v[2] = {0};
  glGetIntegerv(params[10], v);
  gl_log("%s %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv(params[11], &s);
  gl_log("%s %u\n", names[11], (unsigned int)s);
  gl_log("-----------------------------\n");
}


// Reported window size
int g_win_width = 640;
int g_win_height = 480;
// Keep track of framebuffer size for things like the viewport and the mouse cursor
int g_fb_width = 640;
int g_fb_height = 480;

void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
  (void)window;
  g_win_width = width;
  g_win_height = height;
}

void glfw_framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  (void)window;
  g_fb_width = width;
  g_fb_height = height;

  /* TODO: Later update any perspective matrices used here */
}

int main() {
  const GLubyte* renderer;
  const GLubyte* version;
  GLuint vao_1;
  GLuint vao_2;
  GLuint points_vbo;
  GLuint colors_vbo;
  GLuint inverted_points_vbo;
  GLuint inverted_points_colors_vbo;

  /* geometry to use. these are 3 xyz points (9 floats total) to make a triangle */
  GLfloat points[] = { 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f };
  GLfloat colors[] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f };

  GLfloat inverted_points[]        = { 0.0f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f };
  GLfloat inverted_points_colors[] = { 0.8f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, 1.0f, 0.0f, 0.0f };

  const char* vertex_shader     = read_shader( "test.vert" );
  const char* fragment_shader   = read_shader( "test.frag" );
  const char* fragment_shader_2 = read_shader( "test_2.frag" );

  /* GL shader objects for vertex and fragment shader [components] */
  GLuint vert_shader, frag_shader, frag_shader_2;
  /* GL shader programm object [combined, to link] */
  GLuint shader_programm;
  GLuint shader_programm_2;

  if (!restart_gl_log()) { 
    handle_error(); 
  }
  gl_log("starting GLFW\n%s\n", glfwGetVersionString());
  glfwSetErrorCallback(glfw_error_callback);
  /* start GL context and O/S window using the GLFW helper library */
  if ( !glfwInit() ) {
    fprintf( stderr, "ERROR: could not start GLFW3\n" );
    return 1;
  }

  /* Version 4.1 Core is a good default that should run on just about everything. Adjust later to suit project requirements. */
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  glfwWindowHint(GLFW_SAMPLES, 4);

  // Fullscreen
  // GLFWmonitor* mon = glfwGetPrimaryMonitor();
  // const GLFWvidmode* vmode = glfwGetVideoMode(mon);
  // GLFWwindow* window = glfwCreateWindow(vmode->width, vmode->height, "Hello OpenGL", mon, NULL);
  GLFWwindow* window = glfwCreateWindow(g_win_width, g_win_height, "Hello OpenGL", NULL, NULL);
  if ( !window ) {
    fprintf( stderr, "ERROR: could not open window with GLFW3\n" );
    glfwTerminate();
    return 1;
  }
  glfwSetFramebufferSizeCallback( window, glfw_framebuffer_resize_callback );
  glfwSetWindowSizeCallback( window, glfw_window_size_callback );
  glfwMakeContextCurrent( window );

  glfwGetWindowSize( window, &g_win_width, &g_win_height );
  gl_log( "initial window dims %ix%i\n", g_win_width, g_win_height );
  glfwGetFramebufferSize( window, &g_fb_width, &g_fb_height );
  gl_log( "initial framebuffer dims %ix%i\n", g_fb_width, g_fb_height );

  /* start GLEW extension handler */
  glewExperimental = GL_TRUE;
  glewInit();

  /* get version info */
  renderer = glGetString( GL_RENDERER ); /* get renderer string */
  version  = glGetString( GL_VERSION );  /* version as a string */
  printf( "Renderer: %s\n", renderer );
  printf( "OpenGL version supported %s\n", version );
  gl_log("renderer: %s\nversion: %s\n", renderer, version);
  log_gl_params();

  /* tell GL to only draw onto a pixel if the shape is closer to the viewer
  than anything already drawn at that pixel */
  glEnable( GL_DEPTH_TEST ); /* enable depth-testing */
  /* with LESS depth-testing interprets a smaller depth value as meaning "closer" */
  glDepthFunc( GL_LESS );

  /* a vertex buffer object (VBO) is created here. this stores an array of
  data on the graphics adapter's memory. in our case - the vertex points */
  glGenBuffers( 1, &points_vbo );
  glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
  glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), points, GL_STATIC_DRAW );

  glGenBuffers( 1, &colors_vbo );
  glBindBuffer( GL_ARRAY_BUFFER, colors_vbo );
  glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), colors, GL_STATIC_DRAW );

  glGenBuffers( 1, &inverted_points_vbo );
  glBindBuffer( GL_ARRAY_BUFFER, inverted_points_vbo );
  glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), inverted_points, GL_STATIC_DRAW );

  glGenBuffers( 1, &inverted_points_colors_vbo );
  glBindBuffer( GL_ARRAY_BUFFER, inverted_points_colors_vbo );
  glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), inverted_points_colors, GL_STATIC_DRAW );

  /* the vertex array object (VAO) is a little descriptor that defines which
  data from vertex buffer objects should be used as input variables to vertex
  shaders. in our case - use our only VBO, and say 'every three floats is a
  variable' */
  glGenVertexArrays( 1, &vao_1 );
  glBindVertexArray( vao_1 );
  /* "attribute #0 should be enabled when this vao_1 is bound" */
  glEnableVertexAttribArray( 0 );
  /* this VBO is already bound, but it's a good habit to explicitly specify which
  VBO's data the following vertex attribute pointer refers to */
  glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
  /* "attribute #0 is created from every 3 variables in the above buffer, of type
  float (i.e. make me vec3s)" */
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

  glEnableVertexAttribArray( 1 );
  glBindBuffer( GL_ARRAY_BUFFER, colors_vbo );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );

  glGenVertexArrays( 1, &vao_2 );
  glBindVertexArray( vao_2 );
  glEnableVertexAttribArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, inverted_points_vbo );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
  glEnableVertexAttribArray( 1 );
  glBindBuffer( GL_ARRAY_BUFFER, inverted_points_colors_vbo );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );

  /* here we copy the shader strings into GL shaders, and compile them. we
  then create an executable shader 'program' and attach both of the compiled
      shaders. we link this, which matches the outputs of the vertex shader to
  the inputs of the fragment shader, etc. and it is then ready to use */
  vert_shader = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vert_shader, 1, &vertex_shader, NULL );
  glCompileShader( vert_shader );
  frag_shader = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( frag_shader, 1, &fragment_shader, NULL );
  glCompileShader( frag_shader );
  shader_programm = glCreateProgram();
  glAttachShader( shader_programm, frag_shader );
  glAttachShader( shader_programm, vert_shader );
  glLinkProgram( shader_programm );

  frag_shader_2 = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( frag_shader_2, 1, &fragment_shader_2, NULL );
  glCompileShader( frag_shader_2 );
  shader_programm_2 = glCreateProgram();
  glAttachShader( shader_programm_2, frag_shader_2 );
  glAttachShader( shader_programm_2, vert_shader );
  glLinkProgram( shader_programm_2 );

  /* this loop clears the drawing surface, then draws the geometry described
      by the VAO onto the drawing surface. we 'poll events' to see if the window
  was closed, etc. finally, we 'swap the buffers' which displays our drawing
      surface onto the view area. we use a double-buffering system which means
      that we have a 'currently displayed' surface, and 'currently being drawn'
      surface. hence the 'swap' idea. in a single-buffering system we would see
      stuff being drawn one-after-the-other */
  while ( !glfwWindowShouldClose( window ) ) {
    /* wipe the drawing surface clear */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glViewport(0, 0, g_fb_width, g_fb_height);
    glClearColor( 0.6f, 0.6f, 0.8f, 1.0f );
    glUseProgram( shader_programm );
    glBindVertexArray( vao_1 );
    /* draw points 0-3 from the currently bound VAO with current in-use shader */
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    glUseProgram( shader_programm_2 );
    glBindVertexArray( vao_2 );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    /* update other events like input handling */
    glfwPollEvents();
    /* put the stuff we've been drawing onto the display */
    glfwSwapBuffers( window );

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }
  }

  /* close GL context and any other GLFW resources */
  glfwTerminate();
  return 0;
}
