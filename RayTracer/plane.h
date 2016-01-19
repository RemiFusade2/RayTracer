/**********************************************************************
 * plane.h
 *
 * Konubinix  (konubinix@gmail.com)
 * Crée le : dim. 10:28:13 27/12/2009
 ***********************************************************************/
#ifndef __PLANE_H__
#define __PLANE_H__

#include "object.h"
#include <QGLViewer/vec.h>

class Plane : public Object
{
public :
	Plane():width_(0),height_(0){};
	virtual ~Plane() {};
  
	// Accesseurs 
	float width() const { return width_; }
	float height() const { return height_; }

	// Mutateurs 
	void setWidth(float width){width_ = width;};
	void setHeight(float height){height_ = height;};

	virtual void draw() const;
	virtual float boundingRadius() const { return  (width_>0&&height_>0)?(sqrt(width_*width_+height_*height_)/2.0):1.0; }
	virtual bool intersect(const Ray&, Hit& hit) const;
	virtual float quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee,float _light_radius) const;

	virtual void initFromDOMElement(const QDomElement& e);

private:
	float width_;
	float height_;
};

std::ostream& operator<<(std::ostream& o, const Plane&);


#endif /* __PLANE_H__ */
