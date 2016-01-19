#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object.h"

class Ray;
class Hit;


class Triangle : public Object
{
public :
	// Set default parameters. Radius is 1.0, position is 0,0,0. 
	Triangle(qglviewer::Vec A, qglviewer::Vec B, qglviewer::Vec C);
	Triangle();
	virtual ~Triangle() {};

	// Accesseurs 
	float radius() const { return radius_; }
	qglviewer::Vec A() const {return A_;}
	qglviewer::Vec B() const {return B_;}
	qglviewer::Vec C() const {return C_;}
	void setA(qglviewer::Vec A) {A_ = A; updateFrame();}
	void setB(qglviewer::Vec B) {B_ = B; updateFrame();}
	void setC(qglviewer::Vec C) {C_ = C; updateFrame();}
	void setPoints(qglviewer::Vec A, qglviewer::Vec B, qglviewer::Vec C) {A_ = A; B_ = B; C_ = C; updateFrame();}
	//void setRadius(float radius) { radius_ = radius; }
	//void setThirdPoint(float x, float y) { xvertex_ = x; yvertex_ = y; }

	virtual void draw() const;
	virtual float boundingRadius() const { return radius(); }
	virtual bool intersect(const Ray&, Hit& hit) const;

	virtual void initFromDOMElement(const QDomElement& e);

  
private:
  double arccos(double angle);
	void computeUV(Hit& hit) const;
	void displayIntersectionDebug(const Ray& ray, 
				      float dist,
				      Hit& hit,
				      const qglviewer::Vec& OC,
				      const qglviewer::Vec& I,
				      float prod_scal) const;
	void updateFrame();
	float radius_;
	/*float xvertex_;
	float yvertex_;*/
	qglviewer::Vec A_;
	qglviewer::Vec B_;
	qglviewer::Vec C_;
};

std::ostream& operator<<(std::ostream& o, const Triangle&);

#endif // TRIANGLE_H
