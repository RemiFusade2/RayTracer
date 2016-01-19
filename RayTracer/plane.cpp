/**********************************************************************
 * plan.cpp
 *
 * Konubinix  (konubinix@gmail.com)
 * Crée le : dim. 10:28:11 27/12/2009
 ***********************************************************************/
#include <math.h>
#include <assert.h>
#include "plane.h"
#include "material.h"
#include "macros.h"

#define TAILLE 20
#define WIDTH (width_?width_:TAILLE)
#define HEIGHT (height_?height_:TAILLE)

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

void Plane::draw() const
{
	glColor3fv(material().diffuseColor());
	glPushMatrix();
	glMultMatrixd(frame().matrix());
	glBegin(GL_QUADS);
	{
		glVertex3f(WIDTH/2.0,HEIGHT/2.0,0);
		glVertex3f(WIDTH/2.0,-HEIGHT/2.0,0);
		glVertex3f(-WIDTH/2.0,-HEIGHT/2.0,0);
		glVertex3f(-WIDTH/2.0,HEIGHT/2.0,0);
	}
#ifdef DEBUG_INTERSEC
	glBegin(GL_LINES);
	{
		glVertex3f(frame().position().x,
			   frame().position().y,
			   frame().position().z);
		glVertex3f(frame().position().x+frame().orientation().axis().x,
			   frame().position().x+frame().orientation().axis().y,
			   frame().position().x+frame().orientation().axis().z);
	}
	glEnd();
#endif
	glEnd();
	glPopMatrix();
}

void Plane::initFromDOMElement(const QDomElement& e)
{
	Object::initFromDOMElement(e);
	this->width_ = e.attribute("width","0").toFloat();
	this->height_ = e.attribute("height","0").toFloat();
#ifdef DEBUG
	cout << "Initialisation d'un Plan \n\td'orientation : " << frame().orientation() << "\n\tet de position : " << frame().position() << endl;
	cout << "de largeur ";
	if ( width_ ){
		cout << width_ << endl;
	}else{
		cout << "infinie" << endl;
	}
	cout << "de longueur ";
	if ( height_ ){
		cout << height_ << endl;
	}else{
		cout << "infinie" << endl;
	}
#endif
}

bool Plane::intersect(const Ray& ray, Hit& hit) const
{
#ifdef DEBUG_INTERSEC
	std::cout << "TENTATIVE d'intersection du rayon :\n" << ray << "avec le plan\n" << *this << std::endl;
#endif
	// on place le ray dans le répère associé au plan
	Ray ray_associe_plan(frame().coordinatesOf(ray.start()),
			     frame().transformOf(ray.direction()));
	
#ifdef DEBUG_INTERSEC
	std::cout << "rayon dans l'espace du plan : \n" << ray_associe_plan << std::endl;
#endif
	// on vérifie que le ray pointe vers le plan
	if ( ( ray_associe_plan.start().z <= 0 && ray_associe_plan.direction().z <= 0 )
	     || 
	     ( ray_associe_plan.start().z >= 0 && ray_associe_plan.direction().z >= 0 )
		)
	{
#ifdef DEBUG_INTERSEC
		std::cout << "PAS D'INTERSECTION" << std::endl;
#endif
		return false;
	}
	// on détermine le t de l'intersection dans la formule
	// zstart + t*zdir = 0
	float t = -ray_associe_plan.start().z/ray_associe_plan.direction().z;

	if ( hit.time() < t ){
#ifdef DEBUG_INTERSEC
		std::cout << "PAS D'INTERSECTION : " << t << " > " << hit.time() << std::endl;
#endif
		return false;
	}


#ifdef DEBUG_INTERSEC
	std::cout << "Intersection au temps : " << t << std::endl;
#endif
	// Calcul des coordonnées de l'intersection dans le plan et
	// rejet si on dépasse
	float u,v;
	u = ray_associe_plan.start().x+t*ray_associe_plan.direction().x;
	if ( width_ > 0 && (u > width_/2.0 || u < -width_/2.0) ){
#ifdef DEBUG_INTERSEC
		std::cout << "PAS D'INTERSECTION : u = " << u << ", width = " << width_ << std::endl;
#endif
		return false;
	}
	v = ray_associe_plan.start().y+t*ray_associe_plan.direction().y;
	if ( height_ > 0 && (v > height_/2.0 || v < -height_/2.0) ){
#ifdef DEBUG_INTERSEC
		std::cout << "PAS D'INTERSECTION : v = " << v << ", height = " << height_ << std::endl;
#endif
		return false;
	}
	qglviewer::Vec inter_plan = qglviewer::Vec(u,v,0);

#ifdef DEBUG_INTERSEC
	std::cout << "INTERSECTION d'un rayon avec le plan \n"<< *this << "\tau temps : " << t << std::endl;
#endif
 	hit.setTime(t);
	hit.setIntersection(frame().inverseCoordinatesOf(inter_plan));
	if ( ray_associe_plan.start().z <= 0 ){
		hit.setNormal(frame().inverseTransformOf(qglviewer::Vec(0,0,-1)));
	}else{
		hit.setNormal(frame().inverseTransformOf(qglviewer::Vec(0,0,1)));		
	}
	hit.setMaterial(material());
	
	// On envoie les coordonnées de la texture (entre 0 et 1 si la
	// partie est finie)
	if ( width_ > 0 ){
		u = u/width_ + 0.5;
	}
	if ( height_ > 0 ){
		v = v/height_ + 0.5;
	}
#ifdef DEBUG_INTERSEC
	std::cout << "Coordonnées de textures ("<< u << "," << v << ")" << std::endl;
#endif
	hit.setCoord(u,v);
	
	return true;
#ifdef DEBUG
	cerr << "Erreur lors du calcul de l'intersection d'un objet avec un rayon" << endl;
#endif
	return false;
}

float Plane::quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee,float _light_radius) const{
	Hit hit;
	Ray ray(_depart,(_arrivee-_depart).unit());
	float taille_segment = (_arrivee-_depart).norm();
	bool intersected;
	((Plane*)this)->width_ += 2*_light_radius;
	((Plane*)this)->height_ += 2*_light_radius;
	intersected = intersect(ray, hit);
	((Plane*)this)->width_ -= 2*_light_radius;
	((Plane*)this)->height_ -= 2*_light_radius;
	if ( not intersected ){
		return 0;
	}
	if ( taille_segment < hit.time() ){
		return 0;
	}
	qglviewer::Vec intersection_plan = frame().coordinatesOf(ray.start()+(hit.time()*ray.direction()));
	assert(intersection_plan.z < 0.01);
	float depassement_u;
	float depassement_v;
	if ( width_ > 0 ){
		depassement_u = ABS(intersection_plan.x)-width_/2.0;
	}else{
		depassement_u = -1; // on dépasse pas
	}
	if ( height_ > 0 ){
		depassement_v = ABS(intersection_plan.y)-height_/2.0;
	}else{
		depassement_v = -1; // on dépasse pas
	}
	depassement_u = depassement_u/_light_radius;
	depassement_v = depassement_v/_light_radius;
	if ( depassement_u < 0 && depassement_v < 0 ){return 1.0;}
	if ( depassement_u > 0 && depassement_v < 0 ){return 1.0-depassement_u;}
	if ( depassement_u < 0 && depassement_v > 0 ){return 1.0-depassement_v;}
	if ( depassement_u > 0 && depassement_v > 0 ){return MAX(1.0-depassement_v-depassement_u,0);}
	return 1.0;
}


std::ostream& operator<<(std::ostream& o, const Plane& plane){
	o << "position     : " << plane.frame().position() << std::endl;
	o << "orientation  : " <<  plane.frame().orientation() << std::endl;
	return o;
}
