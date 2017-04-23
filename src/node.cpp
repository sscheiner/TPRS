#include <Arduino.h>
#include "node.h"

//constructor
Node::Node(){
  double lat = 0.0;
  double lon = 0.0;
  
}

int Node::getID(){
  return ID;
}

void Node::setID(int x){
  ID = x;
}

int Node::getLayer(){
  return layer;
}

void Node::setLayer(int x){
  layer = x;
}

double  Node::getLat() {
  return lat;
}

void Node::setLat(double x){
  lat = x;
}

double Node::getLon(){
  return lon;
}

void Node::setLon(double x){
  lon = x;
}
