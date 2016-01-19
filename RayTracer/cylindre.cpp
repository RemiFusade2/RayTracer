#include <math.h>
#include "cylindre.h"
#include "material.h"

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

Cylindre::Cylindre()
{
  setBottomRadius(1.0);
  setTopRadius(1.0);
  setHeight(1.0);
  Disque* bottomdisque = new Disque();
  Disque* topdisque = new Disque();	
  setBottomDisque(bottomdisque);
  setTopDisque(topdisque);
}

 Cylindre::~Cylindre()
{
  delete bottomdisque_;
  delete topdisque_;
}

void Cylindre::draw() const
{

	glPushMatrix();

	glMultMatrixd(frame().matrix());
	glColor3fv(material().diffuseColor());
	GLUquadricObj *quadric;
	quadric = gluNewQuadric();

	gluQuadricDrawStyle(quadric, GLU_FILL );
	
	gluCylinder(quadric,bottomradius(),topradius(),height(),18,18);
	//bottomdisque_->draw(true);
	//glTranslatef(0,0,height());
	//topdisque_->draw(true);
	glPopMatrix();
	bottomdisque_->draw();
	topdisque_->draw();	
	
}

void Cylindre::initFromDOMElement(const QDomElement& e)
{
	Object::initFromDOMElement(e);
	float radius;
	
	this->bottomradius_ = e.attribute("bottomRadius","1").toFloat();
	this->topradius_ = e.attribute("topRadius","1").toFloat();
	this->height_ = e.attribute("height","1").toFloat();

	if (e.hasAttribute("radius"))
	{
		radius = e.attribute("radius","1").toFloat();
		this->bottomradius_ = radius;	
		this->topradius_ = radius;
	}
 	
	Disque* bottomdisque = new Disque(this->bottomradius_);
	Disque* topdisque = new Disque(this->topradius_);

	bottomdisque->setMaterial(material());
	topdisque->setMaterial(material());

	Frame* frame_topdisque = new Frame();
	*frame_topdisque = frame();
	frame_topdisque->setPosition(frame_topdisque->position()+Vec(0.0,0.0,height()));
	topdisque->setFrame(*frame_topdisque);
	bottomdisque->setFrame(frame());	

	//bottomdisque->initFromDOMElement(e);
	//topdisque->initFromDOMElement(e);
	//topdisque->frame().setPosition(frametop.position()+Vec(0,0,height()));

//	Vec position_top = frame().position()+Vec(0,0,height());
//	topdisque->frame().setPosition(position_top);

	setBottomDisque(bottomdisque);
	setTopDisque(topdisque);
	//setTopDisque(topdisque);
	

#ifdef DEBUG
	cout << "Initialisation d'un Cylindre de bottomradius " << this->bottomradius_ << ", de topradius " << this->topradius_<< " et de hauteur " << this->height_<< endl;
#endif

}

void Cylindre::displayIntersectionDebug(const Ray& ray,
				      Hit& hit,
				      const qglviewer::Vec& O,
				      const qglviewer::Vec& I,
				      float t) const{
	std::cout << "\nINTERSECTION du cylindre :" << std::endl;
	std::cout << *this << std::endl;
	
	std::cout << "Avec le rayon :" << std::endl;
	std::cout << ray << std::endl;

	std::cout << "TEMPS  : " << hit.time() << std::endl;
	std::cout << "Point d'intersection : " << hit.intersection() << std::endl;
	std::cout << "O  : " << O << std::endl;
	std::cout << "I  : " << I << std::endl;
	std::cout << "t : " << t << std::endl;
}

bool Cylindre::intersect(const Ray& ray, Hit& hit) const
{

	/*bool top = topdisque()->intersect(ray,hit);
	bool bottom = bottomdisque()->intersect(ray,hit);
	return top || bottom;
*/
	// Determination de l'intersection

	// J'appelle I le point intersection s'il existe, O le point d'origine du rayon, D sa direction, R1 et R1 les rayons du cylindre, h sa hauteur
	// C l'origine du repere basé à la base du cylindre

	// on place le ray dans le répère associé au cylindre
	Ray ray_associe_cylindre(frame().coordinatesOf(ray.start()), frame().transformOf(ray.direction()));
	
	Vec I;
	Vec O = ray_associe_cylindre.start();
	const Vec& D = ray_associe_cylindre.direction();
	float R1,R2;

	
	R1 = bottomradius_;
	R2 = topradius_;
	

	float h = height_;

#ifdef DEBUG_INTERSEC
	std::cout << "TENTATIVE d'intersection du rayon :\n" << ray << "avec le cylindre\n" << *this << std::endl;
#endif
	
#ifdef DEBUG_INTERSEC
	std::cout << "rayon dans l'espace du cylindre : \n" << ray_associe_cylindre << std::endl;
#endif


// on vérifie que le ray pointe vers le cylindre
	if ( (frame().position() - ray.start())*ray.direction() < 0){
		
#ifdef DEBUG_INTERSEC
		std::cout << "PAS D'INTERSECTION : direction pointant en arrière" << std::endl;
#endif
		return false;
	}




	// On se place dans les coordonnées du cylindre pour tous les calculs et on a les équations suivantes :
	// Ix = Ox + tDx
	// Iy = Oy + tDy
	// Iz = Oz + tDz
	// Ix² + Iy² = (Iz/h*(R2-R1)+R1)² <=> Ix² + Iy² = Iz²/h²*(R2-R1)²   +  R1² +  2*(R2-R1)*R1*Iz/h

	// on remplace Ix, Iy et Iz dans la 4e equation pour obtenir une equation du 2nd degré en t
	// (Dx² + Dy² - Dz²*(R2-R1)²/h²)*t² + (2OxDx + 	2OyDy - (2OzDz)/h²*(R2-R1)² - 2Dz*(R2-R1)/h)*t + (Ox²+Oy²-Oz²/h²*(R2-R1)²-R1²-2*(R2-R1)*Oz/h = 0

	float a, b ,c;
	if (R1>R2){
		a = D.x*D.x + D.y*D.y -D.z*D.z*(R2-R1)*(R2-R1)/(h*h);
		b = 2*O.x*D.x+2*O.y*D.y - (2*O.z*D.z)*(R2-R1)*(R2-R1)/(h*h) - 2*D.z*R1*(R2-R1)/h;
		c = O.x*O.x+O.y*O.y-O.z*O.z*(R2-R1)*(R2-R1)/(h*h)-2*R1*(R2-R1)*O.z/h -R1*R1;
	}else
	{
		a = D.x*D.x + D.y*D.y -D.z*D.z*(R2-R1)*(R2-R1)/(h*h);
		b = 2*O.x*D.x+2*O.y*D.y - (2*O.z*D.z)*(R2-R1)*(R2-R1)/(h*h) - 2*D.z*R1*(R2-R1)/h;
		c = O.x*O.x+O.y*O.y-O.z*O.z*(R2-R1)*(R2-R1)/(h*h)-2*R1*(R2-R1)*O.z/h -R1*R1;
	
	}
	float delta = b*b-4*a*c;

#ifdef DEBUG_INTERSEC
		cout << "a : " << a << endl << "b : " << b << endl << "c :" << c << endl << "delta : " << delta << endl;
#endif

	

	if (delta < 0)
	{
#ifdef DEBUG_INTERSEC
		cout << "delta est négatif, il n'y a pas de solution" << endl;
#endif
		// pas de solution, il faut vérifier maintenant si on intersecte les disques supérieur ou inférieur
		bool inter_topdisque = topdisque_->intersect(ray,hit);
		bool inter_bottomdisque = bottomdisque_->intersect(ray,hit);
		return (inter_topdisque || inter_bottomdisque);
	}

	
	if (delta > 0)
	{
#ifdef DEBUG_INTERSEC
		cout << "delta est positif, il y a deux solutions" << endl;
#endif
		// deux solutions, le rayon entre et sort par la robe du cylindre
		float t;
		if (a == 0)
		{
			t = -c/b;
		}
		else{
			t = (-b-sqrt(delta))/(2*a);
		}	

		if ( t < 0 ){
#ifdef DEBUG_INTERSEC
			cout << "t < 0 " << endl;
			cout << "PAS D'INTERSECTION avec la robe du cylindre" << endl;
#endif
			return false;
		}
		
		// on calcule le point d'intersection	
		I.x = O.x + t*D.x;
		I.y = O.y + t*D.y;
		I.z = O.z + t*D.z;

#ifdef DEBUG_INTERSEC
		cout <<"I : " << I << endl;
#endif
	

		if (I.z < 0 || I.z > h)
		{
#ifdef DEBUG_INTERSEC
		cout << "t vaut : " << t << endl << "I.z vaut : " << I.z << endl;
		//cout << "PAS D'INTERSECTION avec la robe du cylindre" << endl;
#endif
		bool top = topdisque()->intersect(ray,hit);
		bool bottom = bottomdisque()->intersect(ray,hit);
		return top || bottom;
		}
		

		// on calcule la normale en ce point, dans les coordonnées du cylindre toujours

		float theta = 0;
		Vec normale;
		normale.x = I.x;
		normale.y = I.y;
		normale.z = 0;

		if (normale.norm() == 0)
		{
			cout << "la norme de la normale est nulle" << endl;
		}
	
		if (R1>=R2){
			theta = atan((R1-R2)/h);
			normale = normale * (1/normale.norm())*cos(theta);
			normale = normale + Vec(0,0,sin(theta));
		}
		else
		{
			theta = atan(h/(R2-R1));
			normale = normale * (1/normale.norm())*sin(theta);
			normale = normale - Vec(0,0,cos(theta));
		}
	
		if ( hit.time() > t){
			hit.setTime(t);
			Vec locale = frame().inverseTransformOf(I);
			hit.setIntersection(locale);
			hit.setNormal(frame().inverseTransformOf(normale));
			hit.setMaterial(material());
			computeUV(hit);
#ifdef DEBUG_INTERSEC
			displayIntersectionDebug(ray,hit,O,I,t);
#endif
			topdisque()->intersect(ray,hit);
			bottomdisque()->intersect(ray,hit);
			return true;
		}else{
			return false;
		}

	}
	
	if (delta ==0)
	{
#ifdef DEBUG_INTERSEC
		cout << "delta est nul, il y a une solution" << endl;
#endif
		// une solution, regarder si on intersecte les disques et comparer les time obtenu pour trouver le plus petit
		float t;
		if (a==0)
		{
			t = -c/b;
		} 
		else 
		{
			t=(-b/(2*a));
		}
		

		if (t<0)
		{
			//l'objet est derrière moi
#ifdef DEBUG_INTERSEC
			cout << "On est à l'intérieur de l'objet" << endl; 
#endif
			return false;
		}
		// on calcule le point d'intersection	
		I.x = O.x + t*D.x;
		I.y = O.y + t*D.y;
		I.z = O.z + t*D.z;

		// on calcule la normale en ce point, dans les coordonnées du cylindre toujours

		float theta;
		Vec normale;
		theta = atan((R1-R2)/h);
		normale.x = I.x;
		normale.y = I.y;
		normale.z = 0;

		if (normale.norm() == 0)
		{
			cout << "la norme de la normale est nulle" << endl;
		}

		normale = normale * (1/normale.norm())*cos(theta);

		normale = normale + Vec(0,0,sin(theta));

		if ( hit.time() > t){
			hit.setTime(t);
			hit.setIntersection(frame().inverseCoordinatesOf(I));
			hit.setNormal(frame().inverseCoordinatesOf(normale));
			hit.setMaterial(material());
			computeUV(hit);
#ifdef DEBUG_INTERSEC
			displayIntersectionDebug(ray,hit,O,I,t);
#endif
			topdisque()->intersect(ray,hit);
			bottomdisque()->intersect(ray,hit);
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

float Cylindre::quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee, float _light_radius) const
{
	// On va construire un cylindre de taille br=br+rlr/2, tr = tr+rlr/2, h = h +rlr/2
	// on va créer un rayon d'origine depart et de direction arrivee - depart 
	// et vérifier si ce rayon intersecte les cylindres
	float penombre;


	Cylindre cylindre_penombre;
	cylindre_penombre.setTopRadius(topradius()+_light_radius/2);
	cylindre_penombre.setBottomRadius(bottomradius()+_light_radius/2);
	cylindre_penombre.setHeight(height()+_light_radius/2);
	
	Disque* disque_top = new Disque(cylindre_penombre.topradius());
	Disque* disque_bottom = new Disque(cylindre_penombre.bottomradius());

	disque_bottom->setMaterial(cylindre_penombre.material());
	disque_top->setMaterial(cylindre_penombre.material());

	Frame* frame_topdisque = new Frame();
	*frame_topdisque = cylindre_penombre.frame();
	frame_topdisque->setPosition(frame_topdisque->position()+Vec(0.0,0.0,cylindre_penombre.height()));
	disque_top->setFrame(*frame_topdisque);
	disque_bottom->setFrame(frame());	

	
	cylindre_penombre.setBottomDisque(disque_bottom);
	cylindre_penombre.setTopDisque(disque_top);

	Ray ray;
	Vec dir = (_arrivee-_depart);
	dir = dir / (dir.norm());
	ray.setStart(_depart);
	ray.setDirection(dir);

	Hit hit;

	if (this->intersect(ray,hit))
	{
		penombre = 1;
	}
	else
	{
		if (cylindre_penombre.intersect(ray,hit))
		{
			Vec I = hit.intersection();
			I = cylindre_penombre.frame().coordinatesOf(I);
			penombre = I.z/cylindre_penombre.height();		
		}
		else
		{			
			penombre = 0;
		}
	}

	return penombre;
}

void Cylindre::computeUV(Hit& hit) const
{

	float u,v;
	qglviewer::Vec inter = hit.intersection();
	u = atan2(inter.y,inter.x);
	// mettre sur [0 1] 
	if ( u < 0 ){
		u = u + 2*M_PI;
	}
	u = u/(2*M_PI);
	
	v = inter.z/height();
	hit.setCoord(u,v);
}

std::ostream& operator<<(std::ostream& o, const Cylindre& cylindre){
	o << "centre : " << cylindre.frame().position() << std::endl;
	o << "bottomradius  : " <<  cylindre.bottomradius() << std::endl;
	o << "topradius  : " <<  cylindre.topradius() << std::endl;
	o << "hauteur  : " <<  cylindre.height() << std::endl;
	return o;
}
