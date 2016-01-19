#include <math.h>
#include "sphere.h"
#include "material.h"

/**
   Valeur définissant la zone de pénombre autours de la sphère. Si on
   intersecte une sphère de rayon radius_+SEUIL_INTER autours de celle
   par défaut, on est dans sa pénombre
 */
#define SEUIL_INTER 0.1

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

Sphere::Sphere()
{
  setRadius(1.0);
}

void Sphere::draw() const
{

	glPushMatrix();

	glMultMatrixd(frame().matrix());
	glColor3fv(material().diffuseColor());
	
	gluSphere(gluNewQuadric(),radius(),60,60);
	
	glPopMatrix();
}

void Sphere::initFromDOMElement(const QDomElement& e)
{
	Object::initFromDOMElement(e);
	// Initialisation du radius de la sphere à 1 si rien n'est
	// renseigné
	this->radius_ = e.attribute("radius","1").toFloat();
#ifdef DEBUG
	cout << "Initialisation d'une Sphere de radius " << this->radius_ << endl;
#endif

}

void Sphere::displayIntersectionDebug(const Ray& ray,
				      float dist,
				      Hit& hit,
				      const qglviewer::Vec& OC,
				      const qglviewer::Vec& I,
				      float prod_scal) const{
	std::cout << "\nINTERSECTION de la sphere :" << std::endl;
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

bool Sphere::intersect(const Ray& ray, Hit& hit) const
{
	// Tout d'abord, on détermine si il y a intersection.

	// J'appelle C le centre de la sphere, O le point d'origine du
	// rayon et I le vecteur unitaire suivant sa direction.  En
	// posant || || la norme euclidienne, dist = || OC - <OC/I>I ||
	Vec OC = (this->frame().position()) - ray.start();
	const Vec& I = ray.direction();
	float prod_scal = (OC.x*I.x + OC.y*I.y + OC.z*I.z);
	if ( prod_scal < 0 ){
		// l'objet est derriere moi
		return false;
	}
	float dist = (OC-prod_scal*I).norm();
	// 3 cas à prendre en compte
	// On intersecte pas
	if ( dist > radius_ ){
		return false;
	}

	// on intersecte au bord (intersection sur <OC/I>I de distance
	// prod_scal)
	if ( dist == radius_ ){
		if ( hit.time() > prod_scal){
			hit.setTime(prod_scal);
			hit.setIntersection(ray.start()+prod_scal*I);
			hit.setNormal((ray.start()+prod_scal*I-this->frame().position()).unit());
			hit.setMaterial(material());
			computeUV(hit);
#ifdef DEBUG_INTERSEC
			displayIntersectionDebug(ray,dist,hit,OC,I,prod_scal);
#endif
			return true;
		}else{
			return false;
		}
	}

	// on intersecte dans la sphere
	if ( dist < radius_ ){
		float demi_angle = sqrt(pow(radius_,2)-pow(dist,2));
		float time;
		if ( prod_scal < demi_angle ){
			// je suis dans l'objet
			time = prod_scal + demi_angle;
		}else{
			// Je suis hors de l'objet
			time = prod_scal - demi_angle;
		}
		if ( hit.time() > time ){
			hit.setTime(time);
			hit.setNormal((ray.start()+hit.time()*I-this->frame().position()).unit());
			hit.setIntersection(ray.start()+hit.time()*I);
			hit.setMaterial(material());
			computeUV(hit);
#ifdef DEBUG_INTERSEC
			displayIntersectionDebug(ray,dist,hit,OC,I,prod_scal);
#endif
			return true;
		}else{
			return false;
		}

	}
#ifdef DEBUG
	cerr << "Erreur lors du calcul de l'intersection d'un objet avec un rayon" << endl;
#endif
	return false;
}

float Sphere::quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee, float _light_radius) const
{
	// Pour la sphère, on considère que la lumière passe
	// totalement si elle n'intersecte pas une grosse sphère de
	// rayon _light_radius/2 + radius_ et l'ombre est totale dans
	// la sphère de rayon _light_radius/2-radius_
	Ray ray(_depart,(_arrivee-_depart).unit());
	float __norme = (_arrivee-_depart).norm();
	Hit hit;
	/**
	   \TODO : Enlever cette horreur
	 */
	((Sphere*)this)->radius_ += _light_radius/2.0;
	bool __intersect = intersect(ray,hit);
	((Sphere*)this)->radius_ -= _light_radius/2.0;
	
	// ////////////////////////////////////////
	// 1 : Soit on n'intersecte pas la sphère (ou on intersecte
	// trop tard)
	// ////////////////////////////////////////
	if ( (!__intersect) || (__norme < hit.time()) ){
		return 0.0;
	}

	// ////////////////////////////////////////
	// 2 : Soit on intersecte dans l'ombre
	// ////////////////////////////////////////
	// I : point d'intersection
	// C : centre de la sphère
	Vec IC = this->frame().position() - hit.intersection();
	const Vec& D = ray.direction();	
	float __prodscal = IC.x*D.x+IC.y*D.y+IC.z*D.z;
	float dist = (IC-__prodscal*D).norm();

	float __rayon_petite_sphere = radius_ - _light_radius/2.0;
	if ( dist < __rayon_petite_sphere ){
		// On a tout filtré
		return 1.0;
	}

	// ////////////////////////////////////////
	// 3 : Soit on intersecte dans la pénombre
	// ////////////////////////////////////////
	if ( (dist < (radius_ + _light_radius/2.0)) && (dist >= __rayon_petite_sphere) ){
		// On retourne une fonction linéaire de la distance
		return 1.0-(dist-__rayon_petite_sphere)/(SEUIL_INTER);
	}
	
#ifdef DEBUG
	cerr << "Erreur lors du calcul de l'intersection d'un objet avec un rayon" << endl;
#endif
	return false;
}

void Sphere::computeUV(Hit& hit) const{
	qglviewer::Vec normal = hit.normal();
	// Je prend Z comme axe de hauteur
	float u,v;
	u = atan2(normal.x,normal.y);
	if ( u < 0 ){
		u = u + 2*M_PI;
	}
	u = u/(2*M_PI);
#define OPPOSE (sqrt(pow(normal.y,2)+pow(normal.x,2)))
	v = atan2(normal.z,OPPOSE);
	v= (v+(M_PI/2))/M_PI;
#undef OPPOSE
	hit.setCoord(u,v);
}

std::ostream& operator<<(std::ostream& o, const Sphere& sphere){
	o << "centre : " << sphere.frame().position() << std::endl;
	o << "rayon  : " <<  sphere.radius() << std::endl;
	return o;
}
