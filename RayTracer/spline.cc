/**********************************************************************
 * \file spline.cc
 *
 * \author Konubinix  (konubinix@gmail.com)
 * \date sam. 21:06:23 23/01/2010
 ***********************************************************************/
#include <iostream>
#include "material.h"
#include "spline.h"
#include "assert.h"
#include "macros.h"

void Spline::draw()const{

	glColor3fv(material().diffuseColor());

	const int uOrder = u_order_;
	const int uDegree = uOrder - 1 ;
	const int vOrder = v_order_;
	const int vDegree = vOrder - 1 ;
	GLfloat* sKnotVector = new GLfloat[2*uOrder];
	GLfloat* tKnotVector = new GLfloat[2*vOrder];
	// On crée uOrder points valant 0 au début des paramètres de
	// contrôle et uOrder valant 1 à la fin
	for ( int i = 0 ; i < uOrder ; ++i ) {
		sKnotVector[i] = 0.0;
		sKnotVector[i+uOrder] = 1.0;
	}
	for ( int i = 0 ; i < vOrder ; ++i ) {
		tKnotVector[i] = 0.0;
		tKnotVector[i+vOrder] = 1.0;
	}
	GLUnurbsObj *nurbs = gluNewNurbsRenderer();
//	gluNurbsCallback ( nurbs, GLU_ERROR, erreurNurbs );
	gluBeginSurface ( nurbs );
	gluNurbsSurface ( nurbs,              // Nom de l'objet nurbs
			  uOrder + uDegree + 1,  // Nombre de points sur la grille paramétrique s
			  sKnotVector,       // Vecteur des points s (ne pas oublier de répliquer les points aux bords)
			  vOrder + vDegree + 1,  // Nombre de points sur la grille paramétrique t
			  tKnotVector,       // Vecteur des points s (ne pas oublier de répliquer les points aux bords)
			  3,            // Distance (en nombre de flottants) entre les différents points de contrôle sur u
			  uOrder*3,     // Distance (en nombre de flottants) entre les différents points de contrôle sur v
			  points_,            // Un tableau de flottants représentant les points de contrôle  
			  uOrder,              // Ordre de la spline sur u
			  vOrder,              // Ordre de la spline sur v
			  GL_MAP2_VERTEX_3);  // Type de points utilisés (GL_MAP2_VERTEX_3 ou GL_MAP2_COLOR_4).
	gluEndSurface ( nurbs );
	free(nurbs);
	delete sKnotVector;
	delete tKnotVector;
}

void Spline::initFromDOMElement(const QDomElement& e){
	Object::initFromDOMElement(e);
	this->u_order_ = e.attribute("u_order","0").toFloat();
	this->v_order_ = e.attribute("v_order","0").toFloat();
	assert(u_order_>0 && v_order_>0);
	
	points_ = new GLfloat[u_order_*v_order_*3];

	QDomNode child_node = e.firstChild();
	int indice_points = 0; // pointe toujours sur la prochaine case à remplir
	int taille_points = u_order_*v_order_*3;
	qglviewer::Vec __barycentre;
	while( !child_node.isNull() ) {
		QDomElement child_elem = child_node.toElement(); // try to convert the node to an element.
		if( !child_elem.isNull() ) {
			// the node really is an element.
			if ( child_elem.tagName() == "Vec" ){
				qglviewer::Vec point;
				point.initFromDOMElement(child_elem);
				assert(indice_points < taille_points);
				points_[indice_points] = point.x;
				indice_points++;
				assert(indice_points < taille_points);
				points_[indice_points] = point.y;
				indice_points++;
				assert(indice_points < taille_points);
				points_[indice_points] = point.z;
				indice_points++;
				float __point_number = indice_points/3;
				if ( __point_number == 1 ){
					__barycentre = point;
					bounding_radius_ = 0;
				}else{
					__barycentre = (1.0/__point_number)*((__point_number-1)*__barycentre+point);
					bounding_radius_ = MAX((point-__barycentre).norm(),bounding_radius_);
				}
			}
		}
		child_node = child_node.nextSibling();
	}
	setFrame(qglviewer::Frame(__barycentre,qglviewer::Quaternion()));
#ifdef DEBUG
	std::cout << "Initialisation d'une fonction spline" << std::endl;
#endif
	if ( indice_points < taille_points ){
		std::cerr << "On a initialisé avec moins de points qu'on a dit, on complète avec des 0" << std::endl;
		for ( int i = indice_points ; i < taille_points ; i ++ ){
			points_[i] = 0;
		}
	}
#ifdef DEBUG_SPLINE
	std::cout << *this << std::endl;
#endif
}

std::ostream& operator<<(std::ostream& o, const Spline& s){
	const GLfloat* points = s.points();
	int indice_point;
	o << "order sur u : " << s.uOrder() << ", ordre sur v : " << s.vOrder() << std::endl;
	for (int i = 0 ; i < s.vOrder() ; i++){
		for (int j = 0 ; j < s.uOrder() ; j++){
			indice_point = 3*(i*s.vOrder()+j);
			o << "(" << points[indice_point] << "," << points[indice_point+1] << "," << points[indice_point+2] << ") ";
		}
		o << std::endl;
	}
#ifdef DEBUG_SPLINE
	o << "Boundind radius : " << s.boundingRadius() << std::endl;
	o << "Barycentre      : " << s.frame().position() << std::endl;
#endif
	return o;
}
