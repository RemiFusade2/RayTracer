#include <qmessagebox.h>
#include "light.h"
#include "material.h"
#include "scene.h"
#include "macros.h"

using namespace qglviewer;

void Light::initFromDOMElement(const QDomElement& e)
{
  QDomNode n = e.firstChild();
  while (!n.isNull())
    {
      QDomElement e = n.toElement();
      if (!e.isNull())
	{
	  if (e.tagName() == "Color")
	    color_.initFromDOMElement(e);
	}
      else
	QMessageBox::warning(NULL, "Light XML error", "Error while parsing Light XML document");
      n = n.nextSibling();
    }
}

Color Light::illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const{
	(void)hit;
	(void)eye;
	return color_;
}



////////////////////////////////////////////////////////////////////////////////

DirectionalLight::DirectionalLight()
{
  setDirection(Vec(1.0, 0.0, 0.0));
}

void DirectionalLight::initFromDOMElement(const QDomElement& e)
{
  Light::initFromDOMElement(e);
  
  QDomNode n = e.firstChild();
  while (!n.isNull())
    {
      QDomElement e = n.toElement();
      if (!e.isNull())
	{
	  if (e.tagName() == "Direction")
	    {
	      Vec dir;
	      dir.initFromDOMElement(e);
	      setDirection(dir);
	    }
	}
      else
	QMessageBox::warning(NULL, "DirectionalLight XML error", "Error while parsing DirectionalLight XML document");
      n = n.nextSibling();
    }
#ifdef DEBUG
  std::cout << "Intialisation d'une lumière directionnelle :" << std::endl;
  std::cout << *this << std::endl;
#endif
}

Color DirectionalLight::illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const{
	// N la normal à la surface,
	// V le vecteur de la surface à l'oeil
	// R le vecteur partant de la surface indiquant la reflection de la lumière
	qglviewer::Vec N = hit.bump();
	qglviewer::Vec V = (eye-hit.intersection()).unit();
	float prod_scal = N.x*direction_.x+N.y*direction_.y+N.z*direction_.z;
	qglviewer::Vec R = -2*prod_scal*N+direction_;
	float prod_scal_R_V = R.x*V.x+R.y*V.y+R.z*V.z;

	float diffusion = MAX(0,-prod_scal);
	float specul = pow(MAX(0,prod_scal_R_V),hit.material().specularCoefficient());
	return diffusion*color()*hit.diffuseColor() + specul*hit.material().specularColor();
}

float DirectionalLight::quantityReachedAt(const qglviewer::Vec& pos, const Scene* const scene){
	// On crée un rayon qui démarre sur la pos et qui va dans la
	// direction opposée à celle de la lampe. Si elle "hit"
	// quelque chose, c'est que elle est pas visible
	Ray rayon(pos, -direction_);
	Hit hit;
	// si on croise un objet, il cache tout puisque la lumière
	// directionnelle est située à l'infini
	if ( !scene->intersect(rayon,hit)) {
		return 1.0;
	}else{
		return 0.0;
	}
}

std::ostream& operator<<(std::ostream& o, const DirectionalLight& dl){
	dl.printColor(o);
	o << "direction: " << dl.direction() << std::endl;
	return o;
}

////////////////////////////////////////////////////////////////////////////////

void PointLight::initFromDOMElement(const QDomElement& e)
{
  Light::initFromDOMElement(e);
  this->radius_ = e.attribute("radius","0.1").toFloat();
  this->a = e.attribute("a","1").toFloat();
  this->b = e.attribute("b","0").toFloat();
  this->c = e.attribute("c","0").toFloat();

  QDomNode n = e.firstChild();
  
  while (!n.isNull())
    {
      QDomElement e = n.toElement();
      if (!e.isNull())
	{
	  if (e.tagName() == "Position")
	    position_.initFromDOMElement(e);
	 
	}
      else{
	      QMessageBox::warning(NULL, "PointLight XML error", "Error while parsing PointLight XML document");
      }
      n = n.nextSibling();
    }
#ifdef DEBUG
  std::cout << "Intialisation d'une lumière ponctuelle : " << std::endl;
  std::cout << *this << std::endl;  
#endif
	
	


}

Color PointLight::illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const{
	// N la normal à la surface,
	// V le vecteur de la surface à l'oeil
	// R le vecteur partant de la surface indiquant la reflection de la lumière
	// L le vecteur partant de la surface vers la source
	qglviewer::Vec L = -(hit.intersection() - position_).unit();
	qglviewer::Vec N = hit.bump();
	qglviewer::Vec V = (eye-hit.intersection()).unit();
	float prod_scal = N.x*L.x+N.y*L.y+N.z*L.z;
	qglviewer::Vec R = 2*prod_scal*N-L;
	float prod_scal_R_V = R.x*V.x+R.y*V.y+R.z*V.z;
	float diffusion = MAX(0,prod_scal);
	float specul = pow(MAX(0,prod_scal_R_V),hit.material().specularCoefficient());
	float dist = (hit.intersection()-position_).norm();
	float I = getA() -getB()/dist -getC()/(dist*dist);

//	return (MAX(0,1-dist/POINT_LIGHT_RADIUS))*(diffusion*color()*hit.diffuseColor() + specul*hit.material().specularColor());
	return (MAX(0,I))*(diffusion*color()*hit.diffuseColor() + specul*hit.material().specularColor());
}

bool PointLight::visibleFrom(const qglviewer::Vec& pos, const Scene* const scene,
			     qglviewer::Vec& u, qglviewer::Vec& v,
			     float x, float y){
	Ray rayon(pos, (position_-pos+x*u+y*v).unit());
	Hit hit;
	// Le point est visible s'il n'y a eu aucune intersection
	// entre la scene et la source lumineuse
	if ( !scene->intersect(rayon,hit) ){
		return true;
	}else{
		// Si on croise un objet, on vérifie qu'il est
		// derrière la source lumineuse
		if ( (position_-pos).norm() < hit.time() ){
			// L'objet qu'on intersecte est derrière la source lumineuse
			return true;
		}
	}
	return false;
}

float PointLight::quantityReachedAt(const qglviewer::Vec& pos, const Scene* const scene){
	float __quantity_filtered = scene->quantityIntersected(this->position_,pos,this->radius_);
	return 1.0-__quantity_filtered;
}

void PointLight::draw()const{
	glPushMatrix();
	
	glTranslatef(position_.x,position_.y,position_.z);
	glColor3f(0,1.0,1.0);
	gluSphere(gluNewQuadric(),this->radius_,60,60);
	
	glPopMatrix();

}

std::ostream& operator<<(std::ostream& o, const PointLight& pl){
	pl.printColor(o);
	o << "position : " << pl.position() << std::endl;
	o << "radius   : " <<  pl.radius() << std::endl;
	o << "a : " << pl.getA() << std::endl;
	o << "b : " << pl.getB() << std::endl;
	o << "c : " << pl.getC() << std::endl;
	return o;
}

////////////////////////////////////////////////////////////////////////////////

Color AmbientLight::illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const{
	(void)eye;
	return color()*hit.diffuseColor();
}

void AmbientLight::initFromDOMElement(const QDomElement& e){
	Light::initFromDOMElement(e);
#ifdef DEBUG
	std::cout << "Intialisation d'une lumière ambiente : " << std::endl;
	std::cout << *this << std::endl;
#endif
}

std::ostream& operator<<(std::ostream& o, const AmbientLight& al){
	al.printColor(o);
	return o;
}
