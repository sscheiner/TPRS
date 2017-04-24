#ifndef node_h
#define node_h

class Node{

  private:
  double lat;
  double lon;
  int layer;
  int ID;
  
  
  public:
  Node();
  int getID();
  void setID(int ID);
  int getLayer();
  void setLayer(int layer);
  double getLat();
  void setLat(double lat);
  double getLon();
  void setLon(double lon);

};

#endif
