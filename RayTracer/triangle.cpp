#include <math.h>
#include "triangle.h"
#include "material.h"
#include <qmessagebox.h>

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

Triangle::Triangle() {
  A_ = Vec(0,0,0);
  B_ = Vec(1,0,0);
  C_ = Vec(0,1,0);
  updateFrame();
}

Triangle::Triangle(Vec A, Vec B, Vec C)
{
  A_ = A;
  B_ = B;
  C_ = C;
  updateFrame();
}

void Triangle::draw() const
{
	glColor3fv(material().diffuseColor());
	glBegin(GL_TRIANGLES);
	  glVertex3f(A().x,A().y,A().z);	
	  glVertex3f(B().x,B().y,B().z);	
	  glVertex3f(C().x,C().y,C().z);	
	glEnd();
	//drawAxis();
}

void Triangle::initFromDOMElement(const QDomElement& e)
{
  Vec A, B, C;
	Object::initFromDOMElement(e);
	QDomNode n = e.firstChild();
	while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
	    if (e.tagName() == "A") {
	      A = Vec(e.attribute("x","0").toFloat(),e.attribute("y","0").toFloat(),e.attribute("z","0").toFloat());
	    } else if (e.tagName() == "B") {
	      B = Vec(e.attribute("x","0").toFloat(),e.attribute("y","1").toFloat(),e.attribute("z","0").toFloat());
	    } else if (e.tagName() == "C") {
	      C = Vec(e.attribute("x","0").toFloat(),e.attribute("y","0").toFloat(),e.attribute("z","1").toFloat());
	    }

    } else {
    	QMessageBox::warning(NULL, "Object XML error", "Error while parsing Object XML document");
    }
	  n = n.nextSibling();
	}
  setPoints(A,B,C);
#ifdef DEBUG
	cout << "Initialisation d'un triangle…" << endl;
#endif

}

void Triangle::displayIntersectionDebug(const Ray& ray,
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

bool Triangle::intersect(const Ray& ray, Hit& hit) const
{
#ifdef DEBUG_INTERSEC
	std::cout << "TENTATIVE d'intersection du rayon :\n" << ray << "avec le plan\n" << *this << std::endl;
#endif
	// on place le ray dans le repère associé au plan
	Ray ray_associe_plan(frame().coordinatesOf(ray.start()),
			     frame().transformOf(ray.direction()));
	// on place les sommets du triangle dans le repère associé au plan
  Vec A_associe_plan = frame().coordinatesOf(A());
  Vec B_associe_plan = frame().coordinatesOf(B());
  Vec C_associe_plan = frame().coordinatesOf(C());
	
#ifdef DEBUG_INTERSEC
	std::cout << "rayon dans l'espace du plan : \n" << ray_associe_plan << std::endl;
#endif
	// on vérifie que le ray pointe vers le plan
	if ( ( ray_associe_plan.start().z <= 0 && ray_associe_plan.direction().z <= 0 )
	     || 
	     ( ray_associe_plan.start().z >= 0 && ray_associe_plan.direction().z >= 0 )){
		
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
	
	float u,v;
	u = ray_associe_plan.start().x+t*ray_associe_plan.direction().x;
	v = ray_associe_plan.start().y+t*ray_associe_plan.direction().y;
	qglviewer::Vec inter_plan = qglviewer::Vec(u,v,0);

  // on détermine si le point d'intersection du rayon avec le plan est bien à l'intérieur du triangle
  float x = inter_plan.x;
  float y = inter_plan.y;
  float xA = A_associe_plan.x;
  float yA = A_associe_plan.y;
  float xB = B_associe_plan.x;
  float yB = B_associe_plan.y;
  float xC = C_associe_plan.x;
  float yC = C_associe_plan.y;

  // détermination des poids du barycentre par résolution d'un système
  float det = (xA-xC)*(yB-yC)-(xB-xC)*(yA-yC);
  float poidsA = ((yB-yC)*(x-xC)+(xC-xB)*(y-yC))/det;
  float poidsB = ((yC-yA)*(x-xC)+(xA-xC)*(y-yC))/det;
#ifdef DEBUG_INTERSEC
	std::cout << "Coordonnées barycentriques : " << poidsA << " " << poidsB << " " << 1-poidsA-poidsB << std::endl;
#endif
  
  if (poidsA < 0 || poidsB < 0 || poidsA + poidsB > 1) {
#ifdef DEBUG_INTERSEC
	std::cout << "Le rayon passe à côté du triangle" << std::endl;
#endif
  return false;
  }
  
#ifdef DEBUG_INTERSEC
	std::cout << "INTERSECTION RÉUSSIE d'un rayon avec le triangle \n"<< *this << "\tau temps : " << t << std::endl;
#endif
 	hit.setTime(t);
	hit.setIntersection(frame().inverseCoordinatesOf(inter_plan));
	if ( ray_associe_plan.start().z <= 0 ){
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


void Triangle::updateFrame() {
  Vec I = (A()+B())/2;
  Vec J = (B()+C())/2;
  Vec mediatriceAB = ((B()-A())^(C()-A()))^(B()-A());
  Vec mediatriceBC = ((C()-B())^(C()-A()))^(C()-B());
  float xI = I.x;
  float yI = I.y;
  float zI = I.z;
  float xJ = J.x;
  float yJ = J.y;
  float zJ = J.z;
  float x1 = mediatriceAB.x;
  float y1 = mediatriceAB.y;
  float z1 = mediatriceAB.z;
  float x2 = mediatriceBC.x;
  float y2 = mediatriceBC.y;
  float z2 = mediatriceBC.z;
#ifdef DEBUG
  cout << "Les points du triangle : " << endl << A() << endl << B() << endl << C() << endl << endl;
  cout << "Les milieux : " << endl << I << endl << J << endl;
	cout << "Calcul des médiatrices : " << endl << mediatriceAB << endl << mediatriceBC << endl << endl;
#endif
  // Calcul du centre du cercle circonscrit au triangle
  double k;
  if (x2*y1-x1*y2!=0) 
    k = (-y2*(xJ-xI)+x2*(yJ-yI))/(x2*y1-x1*y2);
  else if (z2*y1-z1*y2!=0)
    k = (-y2*(zJ-zI)+z2*(yJ-yI))/(z2*y1-z1*y2);
  else {
    k = 0;
    radius_ = 0;
    return;
  }
  Vec O = I + k*mediatriceAB;
  frame_.setPosition(O);
  Vec OA = A() - O;
  radius_ = OA.norm();
  Vec Ox = frame().inverseTransformOf(Vec(1,0,0));
  Vec Oy = frame().inverseTransformOf(Vec(0,1,0));
  Vec Oz = frame().inverseTransformOf(Vec(0,0,1));
  // Calcul du premier angle de rotation (d'axe Oz) pour passer du repère monde au repère local
  Vec OAdansplan = Oz^(OA^Oz);
  OAdansplan.normalize();
  double angle1 = arccos(OAdansplan*Ox);
  if (OAdansplan*Oy < 0) angle1 = -angle1;
  Quaternion q1 = Quaternion(frame().transformOf(Oz),angle1);
  frame_.rotate(q1);
  Ox = Quaternion(Oz,angle1).rotate(Ox);
  Oy = Quaternion(Oz,angle1).rotate(Oy);
  // Calcul du deuxième angle de rotation (d'axe Oy)
  double angle2 = arccos((OA/radius_)*OAdansplan);
  if (OA*Oz > 0) angle2 = -angle2;
  Quaternion q2 = Quaternion(frame().transformOf(Oy), angle2);
  frame_.rotate(q2);
  Ox = Quaternion(Oy,angle2).rotate(Ox);
  Oz = Quaternion(Oy,angle2).rotate(Oz);
  // Calcul du troisième angle de rotation (d'axe Ox)
  Vec normaleABC = ((B()-A())^(C()-A()));
  normaleABC.normalize();
  double angle3 = arccos(normaleABC*Oz);
  if (normaleABC*Oy > 0) angle3 = -angle3;
#ifdef DEBUG
  cout << "normale " << (normaleABC) << endl;
  cout << "Oz " << (Oz) << endl;
  cout << "produit scalaire " << (normaleABC*Oz) << endl;
  cout << "angle3 " << arccos(normaleABC*Oz) << " ou " << angle3 << endl;
#endif
  Quaternion q3 = Quaternion(Vec(1,0,0), angle3);
  frame_.rotate(q3);
}


double Triangle::arccos(double angle) {
  if (angle > 1 and angle < 1.01) angle = 1;
  if (angle < -1 and angle > -1.01) angle = -1;
  return acos(angle);
}


void Triangle::computeUV(Hit& hit) const{
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

std::ostream& operator<<(std::ostream& o, const Triangle& triangle){
	o << "centre : " << triangle.frame().position() << std::endl;
	o << "rayon  : " <<  triangle.radius() << std::endl;
	return o;
}
