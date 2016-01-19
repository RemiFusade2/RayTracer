#ifndef DISQUE_H
#define DISQUE_H

#include "object.h"

class Ray;
class Hit;

class Disque : public Object
{
public :

	Disque();
	Disque(float radius);
	virtual ~Disque() {};
  
	// Accesseurs 
	float radius() const { return radius_; }
	void setRadius(float radius) { radius_ = radius; }
	
	void draw(bool cylinder) const;

	virtual void draw() const;
	virtual float boundingRadius() const { return radius(); }
	virtual bool intersect(const Ray&, Hit& hit) const;

	virtual void initFromDOMElement(const QDomElement& e);

  
private:
	// pour texturer les disques à rajouter plus tard peut être ou pas
//	void computeUV(Hit& hit) const;

	// pour aider au debug de intersect
	void displayIntersectionDebug(const Ray& ray, 
				      float dist,
				      Hit& hit,
				      const qglviewer::Vec& OC,
				      const qglviewer::Vec& I,
				      float prod_scal) const;	
	float radius_;

};

std::ostream& operator<<(std::ostream& o, const Disque&);

#endif // DISQUE_H
