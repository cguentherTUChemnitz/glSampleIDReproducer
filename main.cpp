// gl-stuff
#include <GL/glew.h>
#include <GLFW/glfw3.h>  // GLFW helper library
#include <GL/glu.h>

// std stuff
#include <stdio.h>
#include <iostream>
#include <vector>
#include <array>

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(const char* file, int line) {
  GLenum glErr;
  int retCode = 0;

  glErr = glGetError();
  if (glErr != GL_NO_ERROR) {
    printf("glError in file %s @ line %d: %s\n", file, line,
           gluErrorString(glErr));
    retCode = 1;
  }
  return retCode;
}

void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length,
                              const GLchar* message, const void* userParam) {
  printf("0x%X: %s\n", id, message);
}

GLFWwindow* setupOpenGLContextWindow(unsigned int numSamples, bool useGLDebugOutput) {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    throw std::runtime_error("ERROR: could not start GLFW3\n");
  }

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, numSamples);
  GLFWwindow* window = glfwCreateWindow(1, 1, "glSampleIDCheck", NULL, NULL);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("ERROR: could not open window with GLFW3\n");
  }
  glfwMakeContextCurrent(window);

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();

  if (useGLDebugOutput) {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0,
                          GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  }

  if (useGLDebugOutput) {
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
    const GLubyte* version = glGetString(GL_VERSION);    // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
  }

  return window;
}

void checkShaderCompilation(GLuint shader) {
  GLint success = GL_TRUE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    for (auto const& character : errorLog) {
      std::cout << character;
    }
    std::cout << std::endl;
  }
}

GLuint setupShaderProgram() {
  const char* vertex_shader =
      "#version 400\n"
      "in vec3 vp;"
      "void main () {"
      "  vec2 texcoord = vec2( (gl_VertexID << 1) & 2, gl_VertexID & 2 );"
      "  gl_Position = vec4( texcoord * vec2( 2.0f, -2.0f ) + vec2( -1.0f, "
      "1.0f), 0.0f, 1.0f );"
      "}";

  const char* fragment_shader =
      "#version 440\n"
      "layout(binding = 0) uniform atomic_uint atomic_counter;"
      "void main () {"
      //ensure that only one fragment counts, should be redundant due to 1px size of the window
      "  if( ivec2( gl_FragCoord.xy ) == ivec2( 0, 0 ) ){"
      "    for(uint i = 0; i<gl_SampleID; ++i){"
      "      atomicCounterIncrement(atomic_counter);"
      "    }"
      "  }"
      "}";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);
  checkShaderCompilation(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);
  checkShaderCompilation(fs);

  GLuint shader_programm = glCreateProgram();
  glAttachShader(shader_programm, fs);
  glAttachShader(shader_programm, vs);
  glLinkProgram(shader_programm);

  return shader_programm;
}

GLuint* setupBuffers(GLuint& vao, GLuint& atomicBuffer) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glCreateBuffers(1, &atomicBuffer);
  printOpenGLError();

  GLuint const data = 0;
  GLbitfield usageFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT |
                          GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  glNamedBufferStorage(atomicBuffer, sizeof(data), &data, usageFlags);
  printOpenGLError();

  auto mappedBuffer = static_cast<GLuint*>(
      glMapNamedBufferRange(atomicBuffer, 0, sizeof(data), usageFlags));
  printOpenGLError();

  return mappedBuffer;
}

GLint get(GLenum name) {
  GLint value = -1;
  glGetIntegerv(name, &value);
  return value;
}

void testSumOfGLSLSampleIDs(unsigned int numSamples, bool useGLDebugOutput) {
  GLFWwindow* window = setupOpenGLContextWindow(numSamples, useGLDebugOutput);
  printOpenGLError();

#define GLGET(NAME)  std::cout << #NAME ": " << get(NAME) << "\n"

  GLGET(GL_MAX_COLOR_TEXTURE_SAMPLES);
  GLGET(GL_MAX_DEPTH_TEXTURE_SAMPLES);
  GLGET(GL_MAX_FRAMEBUFFER_SAMPLES);

  GLuint vaoID, atomicCounterID;
  auto atomicCounter = setupBuffers(vaoID, atomicCounterID);
  printOpenGLError();

  GLuint shader_programm = setupShaderProgram();
  printOpenGLError();

  std::cout << "atomic Counter initial value: " << atomicCounter[0]
            << " | 0 is correct" << std::endl;

  // wipe the drawing surface clear
  glUseProgram(shader_programm);

  glBindBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounterID, 0,
                    sizeof(GLuint));

  glBindVertexArray(vaoID);
  // draw fullscreen triangle created in vertex shader based on the VertexID
  glDrawArrays(GL_TRIANGLES, 0, 3);
  // update other events like input handling
  glfwPollEvents();
  // put the stuff we've been drawing onto the display
  glfwSwapBuffers(window);

  glFinish();
  unsigned int correctSumOfSampleIDs = 0;
  for (unsigned int i = 0; i < numSamples; ++i) {
    correctSumOfSampleIDs += i;
  }
  std::cout << "sum of gl_SampleIDs: " << atomicCounter[0] << " | "
            << correctSumOfSampleIDs << " is correct" << std::endl;

  // close GL context and any other GLFW resources
  glfwTerminate();
}

int main(int argc, char* argv[]) {
  std::array<unsigned int, 5> listOfSamples{2, 4, 8, 16, 32};
  for (auto const& numSamples : listOfSamples) {
    std::cout << "numSamples: " << numSamples << std::endl;
    testSumOfGLSLSampleIDs(numSamples, false);
    std::cout << std::endl;
  }

  return 0;
}
