/***********************************************************************
 * \file spline.h
 *
 * \author Konubinix  (konubinix@gmail.com)
 * \date sam. 20:56:20 23/01/2010
 ***********************************************************************/
#ifndef __SPLINE_H__
#define __SPLINE_H__

#include <QList>
#include "object.h"

class Spline : public Object
{
public :
	Spline():points_(0),u_order_(0),v_order_(0),bounding_radius_(0){};
	Spline(int _u_order,int _v_order):points_(new GLfloat[_u_order*_v_order*3]),
					  u_order_(_u_order),v_order_(_v_order){}
	virtual ~Spline() {delete points_;};

	// Accesseurs
	int pointsNumber()const{return u_order_*v_order_;};
	const GLfloat* points()const{return (const GLfloat*)(points_);};
	int uOrder()const{return u_order_;};
	int vOrder()const{return v_order_;};

	virtual void draw() const;
	virtual float boundingRadius() const { return bounding_radius_; }
	virtual bool intersect(const Ray& ray, Hit& hit) const {(void)ray;(void)hit;return false;};
	virtual void initFromDOMElement(const QDomElement& e);
	
private:
 	GLfloat* points_;
	int u_order_;
	int v_order_;

	float bounding_radius_;
};

std::ostream& operator<<(std::ostream& o, const Spline&);

#endif /* __SPLINE_H__ */
