#ifndef CYLINDRE_H
#define CYLINDRE_H

#include "object.h"
#include "disque.h"

class Ray;
class Hit;

class Cylindre : public Object
{
public :

	Cylindre();
	virtual ~Cylindre();
  
	// Accesseurs 
	float bottomradius() const { return bottomradius_; }
	float topradius() const { return topradius_; }
	float height() const { return height_; }
	Disque* bottomdisque() const { return bottomdisque_; };
	Disque* topdisque() const { return topdisque_; };

	void setBottomRadius(float bottomradius) { bottomradius_ = bottomradius; }
	void setTopRadius(float topradius) { topradius_ = topradius; }
	void setHeight(float height) { height_ = height; }
	void setBottomDisque(Disque* const bottomdisque) { bottomdisque_ = bottomdisque; };
	void setTopDisque(Disque* const topdisque) { topdisque_ = topdisque;}; 


	virtual void draw() const;
	virtual float boundingRadius() const { return (bottomradius()<topradius()?topradius():bottomradius()); }
	virtual bool intersect(const Ray&, Hit& hit) const;
	virtual float quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee,float _light_radius) const;

	virtual void initFromDOMElement(const QDomElement& e);

  
private:
	void getIntersectVals(float* delta, float* t);
	void computeUV(Hit& hit) const;

	// pour aider au debug de intersect
	void displayIntersectionDebug(const Ray& ray, 
				      Hit& hit,
				      const qglviewer::Vec& O,
				      const qglviewer::Vec& I,
				      float t) const;
	float bottomradius_;
	float topradius_;
	float height_;
	Disque* bottomdisque_;
	Disque* topdisque_;
};

std::ostream& operator<<(std::ostream& o, const Cylindre&);

#endif // CYLINDRE_H
