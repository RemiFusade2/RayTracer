#ifndef HIT_H
#define HIT_H

#include <QGLViewer/frame.h>
#include <QGLViewer/vec.h>

#include "material.h"

class Hit
{
public :
	Hit() { setTime(1.0E5); }

	qglviewer::Vec intersection() const { return intersection_; }
	float time() const { return time_; }
	const Material& material() const { return material_; }
	const qglviewer::Vec& normal() const { return normal_; }
	Color diffuseColor()const {return this->material().diffuseColor(u_,v_);}
	Color perturbation()const {return this->material().perturbation(u_,v_);}

	void setIntersection(const qglviewer::Vec& intersection) { intersection_ = intersection; }
	void setMaterial(const Material& material) { material_ = material; }
	void setTime(float time) { time_ = time; }
	void setCoord(float u, float v) { u_ = u; v_ = v;}
	float u()const{return u_;}
	float v()const{return v_;}
	
	/** 
	    \return La normale bumpée 
	 */
	qglviewer::Vec bump()const;

	void setNormal(qglviewer::Vec normal) { normal_ = normal; }
	void draw();

private:
	qglviewer::Vec intersection_;
	Material material_;
	float time_;
	qglviewer::Vec normal_;
	float u_,v_; // Coordonnées sphériques de la position de hit
};

#endif // HIT_H
