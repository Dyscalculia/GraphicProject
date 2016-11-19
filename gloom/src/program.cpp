// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"



void setup(GLFWwindow* window){
  glfwSetKeyCallback(window, keyboardCallback);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
  printGLError();
}
float rotate = 0;
ChessBoard* board;

void runProgram(GLFWwindow* window){
  setup(window);

  board = createBoard(false);
  Object* triangle = createTriangle();
  Object* wHexagon = createWhiteHex();
  Object* bHexagon = createBlackHex();
  Object* star = createStar();
  Object* circle = createPacman();
  Object* A = createA();
  Object* rectangle = createRectangle();

  Gloom::Shader shader;
  // Square* square = createSquare(glm::vec3(0,0,0), 0.25,0.25,1,0,0);
  shader.makeBasicShader("/home/shomec/h/haakohu/Documents/programmering/tdt4195/graphics/ov4/gloom/gloom/shaders/simple.vert","/home/shomec/h/haakohu/Documents/programmering/tdt4195/graphics/ov4/gloom/gloom/shaders/simple.frag");
  board->squares[0][0]->objects.push_back(triangle);
  board->squares[0][1]->objects.push_back(wHexagon);
  board->squares[0][2]->objects.push_back(star);
  board->squares[0][3]->objects.push_back(circle);
  board->squares[0][4]->objects.push_back(A);
  board->squares[0][5]->objects.push_back(rectangle);
  board->squares[0][6]->objects.push_back(bHexagon);

  setNewChoosenElement();
  while (!glfwWindowShouldClose(window))
  {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glm::mat4 view = glm::translate(glm::mat4(1.f),glm::vec3(0,0.5,-3.5));
      view = glm::rotate(view,-1.f, glm::vec3(1,0,0));
      view = glm::rotate(view,rotate,glm::vec3(0,0,1));
      glm::mat4x4 projection = glm::perspective(glm::radians(45.f),1.f,1.f,100.f);
      glm::mat4x4 vp = projection*view;
      // Draw your scene here
      double delta= getTimeDeltaSeconds()*2;
      updateMovingSquares(delta);
      updateZAnimation(delta);
      shader.activate();
      renderBoard(board,vp);
      shader.deactivate();
      glfwPollEvents();
      glfwSwapBuffers(window);
  }
}

void printMatrix(glm::mat4 matrix) {
	float* values = glm::value_ptr(matrix);

	printf("(%f, %f, %f, %f)\n(%f, %f, %f, %f)\n(%f, %f, %f, %f)\n(%f, %f, %f, %f)\n",
		values[0], values[4], values[8], values[12],
		values[1], values[5], values[9], values[13],
		values[2], values[6], values[10], values[14],
		values[3], values[7], values[11], values[15]);
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
  glVertexAttribPointer(vaid2,4,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(vaid2);

  unsigned int indid = 0;
  glGenBuffers(1,&indid);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indid);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indLen*sizeof(unsigned int),indices, GL_STATIC_DRAW);
  return vaoid;
}

int x = 0;
int y = 0; // Holder rede på hvilken posisjon man er i
int MAX_X=8;
int MAX_Y=5;
bool moving = false;
void setNewChoosenElement(){
  if(board->squares[y][x]->objects.size() == 2){ // det er to elementer på samme flate
    printf("Du kan ikke putte brikken på en annen.\n");
    return;
  }
  moving=true;
  if(board->squares[y][x]->objects.size() > 0){
    Object* object1 = board->squares[y][x]->objects[0];
    object1->choosen = false;
    object1->wantedZ = 0.0;
    object1->goingToPositive = false;
  }

  // Gå til neste element
  bool seen_once = false;
  int start_x = x;
  int start_y = y;
  while(true){
    x++;
    if(x==MAX_X){
      x = 0;
      y++;
      if(y==MAX_Y){
        y = 0;
      }
    }
    if(start_x == x && start_y == y){ // Ingen elementer på brettet...
      if(seen_once==true){
        break;
      }
      else{
        seen_once = true;
      }
    }
    if(board->squares[y][x]->objects.size() > 0){ // Det er et element der
      Object* object = board->squares[y][x]->objects[0];
      object->choosen = true;
      object->wantedZ = 1.25;
      object->goingToPositive = true;
      break;
    }
  }

}

void updateZAnimation(double delta){
  if(moving){
    for(int x = 0;x<MAX_X;x++){
      for(int y =0;y<MAX_Y;y++){
        Square* square = board->squares[y][x];
        for(unsigned int i = 0; i < square->objects.size();i++){
          Object* object = square->objects[i];
          if (object->wantedZ != object->position.z){
            if(object->goingToPositive){
              object->position.z += delta;
              if(object->position.z > object->wantedZ){
                object->position.z = object->wantedZ;
                moving= false;
              }
            }else{
              object->position.z -= delta;
              if(object->position.z < object->wantedZ){
                object->position.z = object->wantedZ;
                moving=false;
              }
            }
          }
        }
      }
    }
  }
}

bool setNewPosition(Square* startSquare, int new_x, int new_y){
  if(startSquare->objects.size()==0){
    printf("ON X: %d, Y: %d\n",x,y);
    printf("There is no elements\n");
    exit(0);
  }
  if(new_x==MAX_X){
    new_x--;
    printf("Invalid route...\n");
    return false;
  }
  if(new_x==-1){
    new_x = 0;
    printf("Invalid route...\n");
    return false;
  }
  if(new_y==MAX_Y){
    new_y--;
    printf("Invalid route...\n");
    return false;
  }
  if(new_y==-1){
    new_y = 0;
    printf("Invalid route...\n");
    return false;
  }
  Object* object;
  if(startSquare->objects.size()==2){
    object = startSquare->objects[1];
    startSquare->objects.erase(startSquare->objects.begin()+1);
  }
  else{
    object = startSquare->objects[0];
    startSquare->objects.erase(startSquare->objects.begin());
  }
  Square* newSquare = board->squares[new_y][new_x];
  newSquare->objects.push_back(object);
  x = new_x;
  y = new_y;
  return true;
}
void setAnimation(int dx, int dy){
  Square* square = board->squares[y][x];
  if(square->objects.size() == 0){
    printf("There is no elements...\n");
    printf("ON X: %d, Y: %d\n",x,y);
    exit(0);
  }
  Object* object;
  if(square->objects.size()==2){
    object = square->objects[1];
  }
  else{
    object = square->objects[0];
  }
  if(dx != 0){
    object->position.x = -dx;
    object->goingToPositive = dx > 0;
  }
  if(dy  != 0){
    object->position.y = -dy;
    object->goingToPositive = dy > 0;
  }
  moving = true;
}

void updateMovingSquares(double delta){
  if(!moving){
    return;
  }
  for(int x = 0;x<MAX_X;x++){
    for(int y =0;y<MAX_Y;y++){
      Square* square = board->squares[y][x];
      if(square->objects.size() > 0){
        for(unsigned int i = 0; i < square->objects.size();i++){
          Object* object = square->objects[i];
          if(object->position.x != 0){
            if(object->goingToPositive){
              object->position.x += delta;
              if(object->position.x >= 0){
                moving = false;
                object->position.x = 0;
              }
            }else{
              object->position.x -= delta;
              if(object->position.x <= 0){
                moving = false;
                object->position.x = 0;
              }
            }
          }
          if(object->position.y != 0){
            if(object->goingToPositive){
              object->position.y += delta;
              if(object->position.y >= 0){
                moving = false;
                object->position.y = 0;
              }
            }else{
              object->position.y -= delta;
              if(object->position.y <= 0){
                moving = false;
                object->position.y = 0;
              }
            }
          }
        }
      }
    }
  }
}
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if(action == GLFW_PRESS && moving){
    printf("There is a object moving. Please wait for next round \n");
    return;
  }
  Square* square = board->squares[y][x];

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    fprintf(stderr, "Could not open GLFW window\n");
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
  {
    setNewChoosenElement();
  }
  if(key == GLFW_KEY_UP && action == GLFW_PRESS){
    if(setNewPosition(square,x,y+1)){
      setAnimation(0,1);
    }

  }
  if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
    if(setNewPosition(square,x,y-1)){
      setAnimation(0,-1);
    }
  }
  if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
    if(setNewPosition(square,x+1,y)){
      setAnimation(1,0);
    }
  }
  if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
    if(setNewPosition(square,x-1,y)){
      setAnimation(-1,0);
    }
  }
  if(key == GLFW_KEY_A && action == GLFW_PRESS){
    rotate += 0.1;
  }
  if(key == GLFW_KEY_D && action == GLFW_PRESS){
    rotate -= 0.1;
  }
}
static std::chrono::steady_clock::time_point _previousTimePoint = std::chrono::steady_clock::now();
double getTimeDeltaSeconds() {
	// Determine the current time
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	// Look up the time when the previous call to this function occurred.
	std::chrono::steady_clock::time_point previousTime = _previousTimePoint;

	// Calculate the number of nanoseconds that elapsed since the previous call to this function
	long long timeDelta = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - _previousTimePoint).count();
	// Convert the time delta in nanoseconds to seconds
	double timeDeltaSeconds = (double)timeDelta / 1000000000.0;

	// Store the previously measured current time
	_previousTimePoint = currentTime;

	// Return the calculated time delta in seconds
	return timeDeltaSeconds;
}
