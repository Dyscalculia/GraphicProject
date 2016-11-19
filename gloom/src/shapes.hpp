#include <program.hpp>
#include <vector>
#include <math.h>

#ifndef PI
  #define PI 3.14159265
#endif
typedef struct Object{
  unsigned int vaoid;
  int indLen;
  float scale;
  bool choosen = false;
  glm::vec3 position = glm::vec3(0,0,0);
  bool goingToPositive;
  float wantedZ = 0;
};

typedef struct Square{
  unsigned int vaoid; // Vertex array object id
  int indLen;
  std::vector<Object*> objects;
  glm::vec3 position;
};

typedef struct ChessBoard {
  std::vector<std::vector<Square*>> squares;

};

ChessBoard* createBoard(bool red);
Square* createSquare(glm::vec3 position, float len, float height,float R, float G, float B);
void renderSquare(Square* square,glm::mat4 mvp);
void renderBoard(ChessBoard* board,glm::mat4 mvp);
void renderObject(Object* object, glm::mat4 vp, glm::mat4 squareMat);
Object* createTriangle(glm::vec3 position, float len,float height, float R, float G, float B);
Object* createObject(float* vertices, int vertLen, unsigned int* indices, int indLen, float* colors, int colorsLen);
std::vector<unsigned int> getExtrudeIndices(unsigned int* indices,int triangleCount);
std::vector<float> extrude(float* vertices,int vertCount,float height);
Object* createHexagon(glm::vec3 position, float height, float R, float G, float B);
float toRadians(float angleDegrees);
Object* createCircle(glm::vec3 position, float height, float R, float G, float B);
Object* createStar(glm::vec3 position, float height, float R, float G, float B);
Object* createA(glm::vec3 position, float height, float R, float G, float B);
Object* createRectangle(glm::vec3 position, float height, float R, float G, float B);

Object* createWhiteHex();
Object* createBlackHex();
Object* createStar();
Object* createPacman();
Object* createA();
Object* createTriangle();
Object* createRectangle();
