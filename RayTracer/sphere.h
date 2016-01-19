#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class Ray;
class Hit;

class Sphere : public Object
{
public :
	// Set default parameters. Radius is 1.0, position is 0,0,0. 
	Sphere();
	virtual ~Sphere() {};
  
	// Accesseurs 
	float radius() const { return radius_; }
	void setRadius(float radius) { radius_ = radius; }

	virtual void draw() const;
	virtual float boundingRadius() const { return radius(); }
	virtual bool intersect(const Ray&, Hit& hit) const;
	virtual float quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee,float _light_radius) const;

	virtual void initFromDOMElement(const QDomElement& e);

  
private:
	void computeUV(Hit& hit) const;
	void displayIntersectionDebug(const Ray& ray, 
				      float dist,
				      Hit& hit,
				      const qglviewer::Vec& OC,
				      const qglviewer::Vec& I,
				      float prod_scal) const;
	float radius_;
};

std::ostream& operator<<(std::ostream& o, const Sphere&);

#endif // SPHERE_H
