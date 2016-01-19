#ifndef MAILLAGE_H
#define MAILLAGE_H

#include "object.h"
#include "triangle.h"

class Ray;
class Hit;


class Maillage: public Object {
public:
  Maillage();
  virtual ~Maillage();
  
  // Accesseurs
  float radius() const;
  int nbTriangles() const {return triangles_.size();}
  virtual void draw() const;
  virtual float boundingRadius() const { return radius(); }
	virtual bool intersect(const Ray& ray, Hit& hit) const;

	virtual void initFromDOMElement(const QDomElement& e);
	
private:
  QList<Triangle*> triangles_;
	void updateFrame();
};


#endif // MAILLAGE_H
