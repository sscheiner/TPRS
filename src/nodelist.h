#include <Arduino.h>
#include <cstdlib> //only use for debugging in VS
#include "node.h"
//set max number of children for each node
#define MAX_SIZE 50


class NodeList{

  private:
  Node list [MAX_SIZE];
  int currentSize;

  public:
  NodeList();
  void addChild(Node child);
  void removeChild(int ID);
  int getCurrentSize();
  int getMaxSize();
  
};

