// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

void setup(GLFWwindow* window){
  glfwSetKeyCallback(window, keyboardCallback);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
  printGLError();
}
void runProgram(GLFWwindow* window){
  setup(window);
  float vertices[] = {
    -1.0,-1.0,0.0,
    0.0 ,-1.0,0.0,
    -0.5,0.0 ,0.0,
    1.0 ,-1.0,0.0,
    0.5 ,0.0 ,0.0,
    0.0 ,1.0 ,0.0
  };
  float colors[] = {
    -0.5,-1.0,0.0,
    0.0 ,-1.0,0.0,
    -0.5,0.0 ,0.0,
    1.0 ,-1.0,0.0,
    0.5 ,0.0 ,0.0,
    0.0 ,1.0 ,0.0
  };
  unsigned int indices[] = {0,1,2,1,3,4,2,4,5};
  int vertLen = 3*6;
  int indLen = 9;
  int colorsLen = 3*6;
  unsigned int vaoid = createVAO(vertices,vertLen,indices,indLen,colors,colorsLen);
  Gloom::Shader shader;
  shader.makeBasicShader("/home/shomec/h/haakohu/Documents/programmering/tdt4195/graphics/ov4/gloom/gloom/shaders/simple.vert","/home/shomec/h/haakohu/Documents/programmering/tdt4195/graphics/ov4/gloom/gloom/shaders/simple.frag");
  while (!glfwWindowShouldClose(window))
  {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Draw your scene here
      shader.activate();
      glBindVertexArray(vaoid);
      glDrawElements(GL_TRIANGLES, indLen, GL_UNSIGNED_INT,0);

      shader.deactivate();

      glfwPollEvents();
      glfwSwapBuffers(window);
  }
}

unsigned int createVAO(float* vertices, int vertLen, unsigned int* indices, int indLen, float* colors, int colorsLen){
  unsigned int vaoid = 0;
  glGenVertexArrays(1,&vaoid);
  glBindVertexArray(vaoid);

  unsigned int vboid = 0;
  glGenBuffers(1,&vboid);
  glBindBuffer(GL_ARRAY_BUFFER, vboid);
  glBufferData(GL_ARRAY_BUFFER, vertLen*sizeof(float), vertices, GL_STATIC_DRAW);
  unsigned int vaid = 0;
  glVertexAttribPointer(vaid,3,GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vaid);

  unsigned int vboid2= 0;
  glGenBuffers(1,&vboid2);
  glBindBuffer(GL_ARRAY_BUFFER,vboid2);
  glBufferData(GL_ARRAY_BUFFER,colorsLen*sizeof(float),colors,GL_STATIC_DRAW);
  unsigned int vaid2 = 1;
  glVertexAttribPointer(vaid2,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(vaid2);

  unsigned int indid = 0;
  glGenBuffers(1,&indid);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indid);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indLen*sizeof(unsigned int),indices, GL_STATIC_DRAW);
  return vaoid;
}

void keyboardCallback(GLFWwindow* window, int key, int scancode,
                      int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
