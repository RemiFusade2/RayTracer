#include <math.h>
#include "cylindre.h"
#include "material.h"

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

Disque::Disque()
{
  setRadius(1.0);
}

Disque::Disque(float radius)
{
  	this->radius_ = radius;
}

void Disque::draw(bool cylinder) const
{
	(void)cylinder;
	GLUquadricObj *quadric;
	quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL );		
	gluDisk(quadric,0,radius_,18,1);	
}

void Disque::draw() const
{
	glPushMatrix();
	glMultMatrixd(frame().matrix());
	glColor3fv(material().diffuseColor());
	//draw(false);
	GLUquadricObj *quadric;
	quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL );		
	gluDisk(quadric,0,radius_,18,1);
	glPopMatrix();
}

void Disque::initFromDOMElement(const QDomElement& e)
{
	Object::initFromDOMElement(e);
	
	if (e.hasAttribute("Radius")){
		this->radius_ = e.attribute("Radius","1").toFloat();
	}
#ifdef DEBUG
	cout << "Initialisation d'un Disque de radius " << this->radius_ << endl;
#endif

}

void Disque::displayIntersectionDebug(const Ray& ray,
				      float dist,
				      Hit& hit,
				      const qglviewer::Vec& OC,
				      const qglviewer::Vec& I,
				      float prod_scal) const{
	std::cout << "\nINTERSECTION du cylindre :" << std::endl;
	std::cout << *this << std::endl;
	
	std::cout << "Avec le rayon :" << std::endl;
	std::cout << ray << std::endl;

	std::cout << "Distance : " << dist << std::endl;
	
	std::cout << "TEMPS  : " << hit.time() << std::endl;
	std::cout << "Point d'intersection : " << hit.intersection() << std::endl;
	std::cout << "OC  : " << OC << std::endl;
	std::cout << "I  : " << I << std::endl;
	std::cout << "prod_scal  : " << prod_scal << std::endl;
}

bool Disque::intersect(const Ray& ray, Hit& hit) const
{
	
#ifdef DEBUG_INTERSEC
	std::cout << "TENTATIVE d'intersection du rayon :\n" << ray << "avec le disque\n" << *this << std::endl;
#endif
	// on place le ray dans le répère associé au disque
	Ray ray_associe_disque(frame().coordinatesOf(ray.start()),
			     frame().transformOf(ray.direction()));
	
#ifdef DEBUG_INTERSEC
	std::cout << "rayon dans l'espace du disque : \n" << ray_associe_disque << std::endl;
#endif
	// on vérifie que le ray pointe vers le disque
	if ( ( ray_associe_disque.start().z <= 0 && ray_associe_disque.direction().z <= 0 )
	     || 
	     ( ray_associe_disque.start().z >= 0 && ray_associe_disque.direction().z >= 0 )){
		
#ifdef DEBUG_INTERSEC
		std::cout << "PAS D'INTERSECTION" << std::endl;
#endif
		
		return false;
	}
	// on détermine le t de l'intersection dans la formule
	// zstart + t*zdir = 0
	float t = -ray_associe_disque.start().z/ray_associe_disque.direction().z;

	if ( hit.time() < t ){
#ifdef DEBUG_INTERSEC
		std::cout << "PAS D'INTERSECTION : " << t << " > " << hit.time() << std::endl;
#endif
		
		return false;
	}


#ifdef DEBUG_INTERSEC
	std::cout << "Intersection au temps : " << t << std::endl;
#endif
	
	float u,v;
	u = ray_associe_disque.start().x+t*ray_associe_disque.direction().x;
	v = ray_associe_disque.start().y+t*ray_associe_disque.direction().y;
	qglviewer::Vec inter_disque = qglviewer::Vec(u,v,0);

	float dist_inter_centre = inter_disque.norm();//(inter_disque - frame().position()).norm();

	if ( dist_inter_centre > radius())
	{
#ifdef DEBUG_INTERSEC
		std::cout << "PAS D'INTERSECTION : " << t << " > " << hit.time() << std::endl;
#endif
		
		return false;
	}


#ifdef DEBUG_INTERSEC
	std::cout << "INTERSECTION d'un rayon avec le disque \n"<< *this << "\tau temps : " << t << std::endl;
#endif
 	hit.setTime(t);
	hit.setIntersection(frame().inverseCoordinatesOf(inter_disque));
	if ( ray_associe_disque.start().z <= 0 ){
		hit.setNormal(frame().inverseTransformOf(qglviewer::Vec(0,0,-1)));
	}else{
		hit.setNormal(frame().inverseTransformOf(qglviewer::Vec(0,0,1)));		
	}
	hit.setMaterial(material());
	hit.setCoord(u,v);
			
	return true;
#ifdef DEBUG
	cerr << "Erreur lors du calcul de l'intersection d'un objet avec un rayon" << endl;
#endif

	return false;
}

std::ostream& operator<<(std::ostream& o, const Disque& disque){
	o << "centre : " << disque.frame().position() << std::endl;
	o << "radius  : " <<  disque.radius() << std::endl;
	return o;
}
