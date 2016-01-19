#include <qmessagebox.h>
#include <QGLViewer/qglviewer.h>

#include "object.h"



void Object::initFromDOMElement(const QDomElement& e)
{
  QDomNode n = e.firstChild();
  while (!n.isNull())
    {      
      QDomElement e = n.toElement();
      if (!e.isNull())
	{
	  if (e.tagName() == "Material")
	    material_.initFromDOMElement(e);
	  else
	    if (e.tagName() == "Frame")
	      {
		frame_.initFromDOMElement(e);

		// Patch : Make sure the orientation is normalized.
		// Absolutely needed to correctly rotate rays.
		qglviewer::Quaternion o = frame_.orientation();
		o.normalize();
		frame_.setOrientation(o);
	      }
	}
      else
	QMessageBox::warning(NULL, "Object XML error", "Error while parsing Object XML document");
      n = n.nextSibling();
    }
}

float Object::quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee,float _light_radius) const {
	(void)_light_radius;
	Hit hit;
	Ray ray(_depart, (_arrivee-_depart).unit());
	// Par défaut, on trace un rayon dans le sens _depart ->
	// _arrivee
	float norme_vecteur = (_arrivee-_depart).norm();
	if ( intersect(ray,hit) && (hit.time() < norme_vecteur) ){
		// Si l'objet intersecte ce rayon avant la fin du
		// segment, il capte la lumière
		return 1.0;
	}else{
		return 0.0;
	}
}

void Object::drawAxis()const{
	glPushMatrix();
	glMultMatrixd(frame().matrix());
	glScalef(2*boundingRadius(),2*boundingRadius(),2*boundingRadius());
	glColor3f(1.0f,0.0f,0.0f);
	QGLViewer::drawArrow(qglviewer::Vec(0,0,0),qglviewer::Vec(1,0,0));
	glColor3f(0.0f,1.0f,0.0f);
	QGLViewer::drawArrow(qglviewer::Vec(0,0,0),qglviewer::Vec(0,1,0));
	glColor3f(0.0f,0.0f,1.0f);
	QGLViewer::drawArrow(qglviewer::Vec(0,0,0),qglviewer::Vec(0,0,1));
	glPopMatrix();
}
