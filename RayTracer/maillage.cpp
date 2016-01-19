#include <math.h>
#include "maillage.h"
#include "material.h"
#include <qmessagebox.h>

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

Maillage::Maillage() {
}

Maillage::~Maillage(){
#ifdef DEBUG
	std::cout << "Destruction du maillage" << std::endl;
#endif	
	QList<Triangle*>::iterator iter_triangles;
	for (iter_triangles = this->triangles_.begin(); iter_triangles != this->triangles_.end(); ++iter_triangles ){
		delete (*iter_triangles);
	}
}

void Maillage::draw() const
{
	// objets
	QList<Triangle*>::const_iterator iter_triangles;
	for (iter_triangles = this->triangles_.constBegin(); iter_triangles != this->triangles_.constEnd(); ++iter_triangles ){
		(*iter_triangles)->draw();
#ifdef DEBUG_LOCAL_AXIS
		(*iter_triangles)->drawAxis();
#endif
	}
}

void Maillage::updateFrame()
{
	// ################################################################################
	// on itère sur les triangles, en faisant la moyenne de leurs
	// centres
	// ################################################################################
	// création d'un vecteur (supposé à (0,0,0) par défaut)
	qglviewer::Vec vecteur;
	
	float nb_triangles_vus = 1;
	QList<Triangle*>::const_iterator iter ;
	for (iter = this->triangles_.constBegin(); iter != this->triangles_.constEnd(); ++iter ){
		// ************************************************************
		// Moy(m1...mn) = ((n-1)*Moy(m1...mn-1)+mn)/n
		// ************************************************************
 		vecteur = (vecteur * (nb_triangles_vus-1) + (*iter)->frame().position())/(nb_triangles_vus);
		nb_triangles_vus++;
	}
#ifdef DEBUG
	std::cout << "Centre du maillage : " << std::endl << vecteur << std::endl;
#endif
  frame_.setPosition(vecteur);
}

float Maillage::radius() const
{
	// ##################################################
	// on itère sur les triangles, en prenant le max de leurs radius
	// ################################################################################
	qglviewer::Vec position_triangle;
	float radius = 0;
	float distance_au_centre;
	
	QList<Triangle*>::const_iterator iter;
	for (iter = this->triangles_.constBegin() ; iter != this->triangles_.constEnd(); ++iter ){
		distance_au_centre = ((*iter)->frame().position()-frame().position()).norm();
		// ************************************************************
		// Moy(m1...mn) = ((n-1)*Moy(m1...mn-1)+mn)/n
		// ************************************************************
 		radius = std::max(radius, distance_au_centre+(*iter)->boundingRadius());
	}
#ifdef DEBUG
	std::cout << "Rayon de la scène : " << std::endl << radius << std::endl;
#endif
	return radius;
}

bool Maillage::intersect(const Ray& ray, Hit& hit) const {
	QList<Triangle*>::const_iterator iter;
	for (iter = this->triangles_.constBegin() ; iter != this->triangles_.constEnd(); ++iter ){
	  if ((*iter)->intersect(ray, hit)) {
	    hit.setMaterial(material());
      #ifdef DEBUG_INTERSEC
	      std::cout << "Le rayon rencontre le maillage !" << endl;
      #endif
	    return true;
	  }
  }  
  return false;
}

void Maillage::initFromDOMElement(const QDomElement& e) {
  QList<Vec*> vertices;
  Vec* vecteur;
  Vec A, B, C;
	Object::initFromDOMElement(e);
	QDomNode n = e.firstChild();
	while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
	    /*if (e.tagName() == "Triangle") {  
    	  Triangle* const triangle = new Triangle();
        triangle->initFromDOMElement(e);
        this->triangles_.append(triangle);
      } else */
      if (e.tagName() == "Vertex") {
        vecteur = new Vec(e.attribute("x","0").toFloat(),e.attribute("y","0").toFloat(),e.attribute("z","0").toFloat());
        vertices.append(vecteur);
      } else if (e.tagName() == "Triangle") {
        A = *vertices.at(e.attribute("A","1").toInt());
        B = *vertices.at(e.attribute("B","1").toInt());
        C = *vertices.at(e.attribute("C","1").toInt());
        cout << "TRIANGLE : " << endl << A << endl << B << endl << C << endl;
        Triangle* const triangle = new Triangle(A,B,C);
        triangle->setMaterial(material());
        this->triangles_.append(triangle);
      }
    } else {
    	QMessageBox::warning(NULL, "Object XML error", "Error while parsing Object XML document");
    }
	  n = n.nextSibling();
  }
  updateFrame();
}
