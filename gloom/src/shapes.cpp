
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

std::vector<float> createColors(int count, float R,float G, float B){
  std::vector<float> nw = {};
  for(int i = 0;i<count;i++){
    nw.push_back(R);
    nw.push_back(G);
    nw.push_back(B);
    nw.push_back(1.0f);
  }
  return nw;
}
Square* createSquare(glm::vec3 position, float len, float height,float R, float G, float B){
  float oVertices[]{
    position.x,position.y,position.z,
    position.x+len,position.y,position.z,
    position.x+len,position.y+len,position.z,
    position.x,position.y+len,position.z,
  };
  int vertCount = 4;
  std::vector<float> genVert = extrude(oVertices,vertCount,height);
  int vertLen = genVert.size();
  float* vertices = &genVert[0];
  std::vector<float> genColors = createColors(vertCount*2,R,G,B);
  int colorsLen = genColors.size();
  float* colors = &genColors[0];
  unsigned int oIndices[] = {
    0,1,3,
    1,2,3
  };
  std::vector<unsigned int> genInd = getExtrudeIndices(oIndices,2);
  unsigned int* indices = &genInd[0];
  int indLen = genInd.size();
  unsigned int vaoid = createVAO(vertices, vertLen, indices, indLen, colors, colorsLen);
  Square* square = new Square;
  square->vaoid = vaoid;
  square->indLen = indLen;
  square->position = position;
  return square;
}
// Lager et board fra -1.0 til 1.0 i x retning
ChessBoard* createBoard(bool red){
  ChessBoard* board = new ChessBoard;
  float x_pos = -1.0;
  float y_pos = -1.0;

  for(int i = 0; i < 5;i++){
    x_pos = -1.0;
    std::vector<Square*> newRow = {};
    for(int j = 0; j < 8; j++){
      Square* square;
      if(red){
        square = createSquare(glm::vec3(x_pos,y_pos,-1),0.25,0.2,1.f,0.f,0.f);
      }
      else{
        square = createSquare(glm::vec3(x_pos,y_pos,-1),0.25,0.2,0.f,0.f,1.f);
      }
      red = ! red;
      newRow.push_back(square);
      x_pos += 0.25;
    }
    board->squares.push_back(newRow);
    y_pos += 0.25;
    red = ! red;
  }
  return board;
}

void renderSquare(Square* square,glm::mat4 vp){
  glBindVertexArray(square->vaoid);
  glUniformMatrix4fv(3,1,GL_FALSE,&vp[0][0]);
  glDrawElements(GL_TRIANGLES,square->indLen,GL_UNSIGNED_INT,0);
  glm::mat4 transMat = glm::translate(glm::mat4(1),square->position);
  for(unsigned int i = 0; i< square->objects.size();i++){
    renderObject(square->objects[i],vp,transMat);
  }
}
//Create new vertices for the vector
std::vector<float> extrude(float* vertices,int vertCount,float height){ //Får inn alle vertices som ikke er extruded og antall på disse.
  std::vector<float> nw = {};
  for(int i = 0; i<vertCount;i++){
    int j = i*3;
    nw.push_back(vertices[j]);
    nw.push_back(vertices[j+1]);
    nw.push_back(vertices[j+2]);
    nw.push_back(vertices[j]);
    nw.push_back(vertices[j+1]);
    nw.push_back(vertices[j+2]+height);
  }
  return nw;
}
// Forventer at indices er i klokke rekkefølge
std::vector<unsigned int> getExtrudeIndices(unsigned int* indices,int triangleCount){ // forventer å få en liste med 3*n elementer, hvor en trekant er gruppert.
  std::vector<unsigned int> nw = {};
  //indices baserer seg på indekser for nodene før de er extruded. Ekstruded komponenten til en node i er i +1,
  for(int i = 0; i< triangleCount;i++){
    int j = i*3;
    unsigned int node0 = indices[j]*2;
    unsigned int node1 = indices[j+1]*2;
    unsigned int node2 = indices[j+2]*2;
    //bot
    nw.push_back(node0); nw.push_back(node2); nw.push_back(node1);
    //top
    nw.push_back(node0+1); nw.push_back(node1+1); nw.push_back(node2+1);
    //leftside
    nw.push_back(node0); nw.push_back(node2+1); nw.push_back(node2);
    nw.push_back(node0); nw.push_back(node0+1); nw.push_back(node2+1);
    //right side
    nw.push_back(node1); nw.push_back(node2); nw.push_back(node2+1);
    nw.push_back(node1); nw.push_back(node2+1); nw.push_back(node1+1);
    //front
    nw.push_back(node0); nw.push_back(node1); nw.push_back(node1+1);
    nw.push_back(node0); nw.push_back(node1+1); nw.push_back(node0+1);
  }
  return nw;
}
Object* createObject(float* vertices, int vertCount,float height, unsigned int* indices, int triangleCount, float R, float G, float B){
  std::vector<float> vertGen = extrude(vertices, vertCount, height);
  vertices = &vertGen[0];
  int vertLen = vertGen.size();
  std::vector<float> genColors = createColors(vertCount*2, R,G,B);
  float* colors = &genColors[0];
  int colorsLen = genColors.size();
  std::vector<unsigned int> genIndices = getExtrudeIndices(indices, triangleCount);
  indices = &genIndices[0];
  int indLen = genIndices.size();
  unsigned int vaoid = createVAO(vertices, vertLen, indices,indLen,colors,colorsLen);
  Object* triangle = new Object;
  triangle->vaoid = vaoid;
  triangle->indLen = indLen;
  triangle->scale = 0.25;
  return triangle;
}
Object* createTriangle(glm::vec3 position, float len,float height, float R, float G, float B){
  float vertices[] = {
    position.x,position.y,position.z,
    position.x+len,position.y,position.z,
    position.x+len/2,position.y+len,position.z,
  };
  int vertCount = 3;
  unsigned int indices[] = {
    0,1,2
  };
  int triangleCount = 1;
  return createObject(vertices,vertCount,height,indices,triangleCount,R,G,B);
}
Object* createHexagon(glm::vec3 position, float height, float R, float G, float B){
  float vertices[] = {
    position.x,position.y+0.5f,position.z,
    position.x+0.25f,position.y+1.f,position.z,
    position.x+0.75f,position.y+1.f,position.z,
    position.x+1.f,position.y+0.5f,position.z,
    position.x+0.75f,position.y,position.z,
    position.x+0.25f,position.y,position.z
  };
  int vertCount = 6;
  unsigned int indices[] = {
    5,1,0,
    5,4,1,
    4,2,1,
    4,3,2
  };
  int triangleCount = 4;
  return createObject(vertices,vertCount,height, indices, triangleCount, R,G,B);
}
Object* createStar(glm::vec3 position, float height, float R, float G ,float B){
  float vertices[] = {
    position.x+0.f,position.y+0.6f,position.z,
    position.x+0.25f,position.y+0.66f,position.z,
    position.x+0.5f,position.y+1.f,position.z,
    position.x+0.75f,position.y+0.66f,position.z,
    position.x+1.f,position.y+0.6f,position.z,
    position.x+0.75f,position.y+0.33f,position.z,
    position.x+0.8f,position.y+0.f,position.z,
    position.x+0.5f,position.y+0.15f,position.z,
    position.x+0.2f,position.y+0.f,position.z,
    position.x+0.25f,position.y+0.33f,position.z
  };
  int vertCount = 10;
  unsigned int indices[] = {
    1,0,9,
    1,3,2,
    3,5,4,
    5,7,6,
    7,9,8,
    1,9,7,
    1,7,5,
    1,5,3
  };
  int triangleCount = 8;
  return createObject(vertices, vertCount, height, indices, triangleCount, R,G,B);
}
Object* createCircle(glm::vec3 position, float height, float R, float G, float B){
  std::vector<float> vertices = {};
  int vertCount = 1;
  vertices.push_back(position.x+0.5);vertices.push_back(position.y+0.5);vertices.push_back(position.z);
  for(int i = -90;i<181;i+=2){
    float x = position.x+0.5f*cos(toRadians(i))+0.5;
    float y = position.y+0.5f*sin(toRadians(i))+0.5;
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(position.z);
    vertCount++;
  };
  int triangleCount = vertCount-2;
  std::vector<unsigned int> indices = {};
  for(int i = 0; i<triangleCount; i++){
    indices.push_back(0);
    indices.push_back(i+1);
    indices.push_back(i+2);
  }
  return createObject(&vertices[0],vertCount,height,&indices[0],triangleCount,R,G,B);
}
Object* createA(glm::vec3 position, float height, float R, float G, float B){
  float vertices[] = {
    position.x,position.y,position.z,
    position.x+0.5f,position.y+1.f,position.z,
    position.x+1.f,position.y,position.z,
    position.x+0.75f,position.y,position.z,
    position.x+0.5f,position.y+0.6f,position.z,
    position.x+0.25f,position.y,position.z
  };
  int vertCount = 6;
  unsigned int indices[] = {
    0,5,1,
    1,5,4,
    4,3,2,
    1,4,2
  };
  int triangleCount = 4;
  return createObject(vertices,vertCount,height,indices,triangleCount,R,G,B);
}
Object* createRectangle(glm::vec3 position, float height, float R, float G, float B){
  float vertices[] = {
    position.x,position.y,position.z,
    position.x+0.33f,position.y+1.f,position.z,
    position.x+1.f,position.y+1.f,position.z,
    position.x+0.66f,position.y,position.z
  };
  int vertCount = 4;
  unsigned int indices[] = {
    0,3,1,
    1,3,2
  };
  int triangleCount = 2;
  return createObject(vertices,vertCount,height,indices,triangleCount,R,G,B);
}

void renderObject(Object* object, glm::mat4 vp, glm::mat4 squareMat){
  glBindVertexArray(object->vaoid);
  glm::mat4 scale = glm::scale(glm::vec3(object->scale));
  glm::mat4 trans = glm::translate(glm::mat4(1), object->position);

  glm::mat4 mvp = vp* squareMat * scale * trans; //ganger med trans sist slik at den blir riktig i forhold til scale..
  glUniformMatrix4fv(3,1,GL_FALSE,&mvp[0][0]);
  glDrawElements(GL_TRIANGLES,object->indLen,GL_UNSIGNED_INT,0);
}

void renderBoard(ChessBoard* board,glm::mat4 mvp){
  for(unsigned int i=0; i < board->squares.size();i++){
    for(unsigned int j=0;j < board->squares[i].size();j++){
      renderSquare(board->squares[i][j],mvp);
    }
  }
}
float toRadians(float angleDegrees){
  return angleDegrees * (PI / 180.0);
}

Object* createWhiteHex(){
  return createHexagon(glm::vec3(0,0,0.75),0.25,1.f,1.f,1.0f);
}

Object* createBlackHex(){
  return createHexagon(glm::vec3(0,0,0.75),0.25,0.0f,0.0f,0.0f);
}
Object* createStar(){
  return createStar(glm::vec3(0,0,0.75),0.25,0.0f,0.0f,0.7f);
}
Object* createPacman(){
  return createCircle(glm::vec3(0,0,0.75),0.25,0.7f,0.0f,0.0f);
}
Object* createA(){
  return createA(glm::vec3(0,0,0.75),0.25,1.f,1.f,0.0f);

}
Object* createTriangle(){
  return createTriangle(glm::vec3(0,0,0.75),1,0.25,0.5f,0.0f,0.5f);
}
Object* createRectangle(){
  return createRectangle(glm::vec3(0,0,0.75),0.25,0.0f,1.f,0.0f);
}
