#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define ARRAY_SIZE( arr ) ( sizeof( arr ) / sizeof( ( arr )[0] ) )
#define handle_error()                                                                                                                                                                                 \
  ( {                                                                                                                                                                                                  \
    printf( "Error %s\n", strerror( errno ) );                                                                                                                                                         \
    exit( -1 );                                                                                                                                                                                        \
  } )

char* read_shader( char* filepath ) {
  FILE* fp = fopen( filepath, "r" );
  if ( !fp ) { handle_error(); }

  size_t buffer_size = 4096;
  char* buffer       = calloc( buffer_size, 1 );
  fread( buffer, buffer_size, sizeof( *buffer ), fp );

  return buffer;
}

int main() {
  GLFWwindow* window = NULL;
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

  GLfloat inverted_points[] = { 0.0f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f };
  GLfloat inverted_points_colors[] = { 0.8f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, 1.0f, 0.0f, 0.0f };

  const char* vertex_shader   = read_shader( "test.vert" );
  const char* fragment_shader = read_shader( "test.frag" );
  const char* fragment_shader_2  = read_shader("test_2.frag"); 

  /* GL shader objects for vertex and fragment shader [components] */
  GLuint vert_shader, frag_shader, frag_shader_2;
  /* GL shader programm object [combined, to link] */
  GLuint shader_programm;
  GLuint shader_programm_2;

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

  window = glfwCreateWindow( 640, 480, "Hello OpengGL", NULL, NULL );
  if ( !window ) {
    fprintf( stderr, "ERROR: could not open window with GLFW3\n" );
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent( window );

  /* start GLEW extension handler */
  glewExperimental = GL_TRUE;
  glewInit();

  /* get version info */
  renderer = glGetString( GL_RENDERER ); /* get renderer string */
  version  = glGetString( GL_VERSION );  /* version as a string */
  printf( "Renderer: %s\n", renderer );
  printf( "OpenGL version supported %s\n", version );

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

  glGenVertexArrays(1, &vao_2);
  glBindVertexArray(vao_2);
  glEnableVertexAttribArray( 0 );
  glBindBuffer(GL_ARRAY_BUFFER, inverted_points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
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
    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    glUseProgram( shader_programm );
    glBindVertexArray( vao_1 );
    /* draw points 0-3 from the currently bound VAO with current in-use shader */
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    glUseProgram( shader_programm_2 );
    glBindVertexArray(vao_2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    /* update other events like input handling */
    glfwPollEvents();
    /* put the stuff we've been drawing onto the display */
    glfwSwapBuffers( window );
  }

  /* close GL context and any other GLFW resources */
  glfwTerminate();
  return 0;
}
