#include <qfile.h>
#include <algorithm> // pour le max
#include <qdom.h>
#include <qiodevice.h>
#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "cylindre.h"
#include "maillage.h"
#include "spline.h"
#include "constants.h"

using namespace std;

Scene::~Scene(){
#ifdef DEBUG
	std::cout << "Vidage de la scène" << std::endl;
#endif	
	QList<Object*>::iterator iter_objects;
	for (iter_objects = this->objects_.begin(); iter_objects != this->objects_.end(); ++iter_objects ){
		delete (*iter_objects);
	}

	QList<Light*>::iterator iter_lights;
	for (iter_lights = this->lights_.begin(); iter_lights != this->lights_.end(); ++iter_lights ){
		delete (*iter_lights);
	}
}

void Scene::draw() const
{
	// objets
	QList<Object*>::const_iterator iter_objects;
	for (iter_objects = this->objects_.constBegin(); iter_objects != this->objects_.constEnd(); ++iter_objects ){
		(*iter_objects)->draw();
#ifdef DEBUG_LOCAL_AXIS
		(*iter_objects)->drawAxis();
#endif
	}
	
	// objets
	QList<Light*>::const_iterator iter_lights;
	for (iter_lights = this->lights_.constBegin(); iter_lights != this->lights_.constEnd(); ++iter_lights ){
		(*iter_lights)->draw();
	}
	
	// camera
	camera_.draw(1);
}

void Scene::addObject(Object* o)
{
	this->objects_.append(o);
}

void Scene::addLight(Light* l)
{
	this->lights_.append(l);
}

void Scene::loadFromFile(const QString& filename)
{
#ifdef DEBUG
	cout << "Chargement de la scene '" << filename.toStdString() << "'" << endl;
#endif
	// ################################################################################
	// Création et initialisation du document qui va contenir
	// toute la structure et du fichier qu'on va lire
	// ################################################################################
	QDomDocument doc(filename);
	QFile file(filename);
	if ( !file.open( QIODevice::ReadOnly ) )
		return;
	if ( !doc.setContent( &file ) ) {
		file.close();
		return;
	}
	file.close();
	
	// ################################################################################
	// Lecture du document qui contient mtn toute la structure
	// ################################################################################
	QDomElement docElem = doc.documentElement();
	// ************************************************************
	// Traitement des attributs de la scene
	// ************************************************************
	float r,g,b;
	r = docElem.attribute("red","0.5").toFloat();
	g = docElem.attribute("green","0.5").toFloat();
	b = docElem.attribute("blue","0.5").toFloat();
	this->backgroundColor_ = Color(r,g,b);

	// ************************************************************
	// Parcours de tous les fils de premier niveau
	// ************************************************************
	QDomNode n = docElem.firstChild();
	while( !n.isNull() ) {
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if( !e.isNull() ) {
			this->initElement(e);// the node really is an element.
		}
		n = n.nextSibling();
	}
}

void Scene::initElement(QDomElement& e){
	if ( e.tagName() == "Sphere" ){
		Sphere* const sphere = new Sphere();
		sphere->initFromDOMElement(e);
		this->addObject(sphere);
	}else if ( e.tagName() == "Triangle" ){
		Triangle* const triangle = new Triangle();
		triangle->initFromDOMElement(e);
		this->addObject(triangle);
	}else if ( e.tagName() == "Maillage" ){
		Maillage* const maillage = new Maillage();
		maillage->initFromDOMElement(e);
		this->addObject(maillage);
	}else if ( e.tagName() == "Plane" ){
		Plane* const plane = new Plane();
		plane->initFromDOMElement(e);
		this->addObject(plane);
	}else if ( e.tagName() == "Cylinder" ){
		Cylindre* const cylindre = new Cylindre();
		cylindre->initFromDOMElement(e);
		this->addObject(cylindre);
	}else if ( e.tagName() == "Camera" ){
		Camera* const camera = new Camera();
		camera->initFromDOMElement(e);
		this->setCamera(*camera);
	}else if ( e.tagName() == "AmbientLight" ){
		AmbientLight* const ambient_light = new AmbientLight();
		ambient_light->initFromDOMElement(e);
		this->addLight(ambient_light);
	}else if ( e.tagName() == "DirectionalLight" ){
		DirectionalLight* const directional_light = new DirectionalLight();
		directional_light->initFromDOMElement(e);
		this->addLight(directional_light);
	}else if ( e.tagName() == "PointLight" ){
		PointLight* const point_light = new PointLight();
		point_light->initFromDOMElement(e);
		this->addLight(point_light);
	}else if ( e.tagName() == "Spline" ){
		Spline* const spline = new Spline();
		spline->initFromDOMElement(e);
		this->addObject(spline);
	}else{
#ifdef DEBUG
		cout << "Element non reconnu : '"<< e.tagName().toStdString() << "'" << endl;
#endif
	}
}

qglviewer::Vec Scene::center() const
{
	// ################################################################################
	// on itère sur les objets, en faisant la moyenne de leurs
	// centres
	// ################################################################################
	// création d'un vecteur (supposé à (0,0,0) par défaut)
	qglviewer::Vec vecteur;
	
	float nb_objects_vus = 1;
	QList<Object*>::const_iterator iter ;
	for (iter = this->objects_.constBegin(); iter != this->objects_.constEnd(); ++iter ){
		// ************************************************************
		// Moy(m1...mn) = ((n-1)*Moy(m1...mn-1)+mn)/n
		// ************************************************************
 		vecteur = (vecteur * (nb_objects_vus-1) + (*iter)->frame().position())/(nb_objects_vus);
		nb_objects_vus++;
	}
#ifdef DEBUG
	std::cout << "Centre de la scène : " << std::endl << vecteur << std::endl;
#endif
	return vecteur;
}

float Scene::radius(const qglviewer::Vec& center) const
{
	// ##################################################
	// on itère sur les objets, en prenant le max de leurs radius
	// ################################################################################
	qglviewer::Vec position_objet;
	float radius = 0;
	float distance_au_centre;
	
	QList<Object*>::const_iterator iter;
	for (iter = this->objects_.constBegin() ; iter != this->objects_.constEnd(); ++iter ){
		position_objet=(*iter)->frame().position();
		distance_au_centre = (position_objet-center).norm();
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

bool Scene::intersect(const Ray& ray, Hit& hit) const {
	// On parcours tous les objets et on calcul leur intersection
	bool res = false;
	QList<Object*>::const_iterator iter;
	for (iter = this->objects_.constBegin(); iter != this->objects_.constEnd(); ++iter ){
		res |= (*iter)->intersect(ray,hit);
	}
	hit.setIntersection(hit.intersection()+EPSILON*hit.normal());
	return res;	
}

float Scene::quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee,float _light_radius) const {
	// On parcourt tous les objets et on calcul leur intersection
	qglviewer::Vec __direction = (_depart-_arrivee).unit();
	float res = 0.0;
	QList<Object*>::const_iterator iter;
	float quantity;
	for (iter = this->objects_.constBegin(); iter != this->objects_.constEnd(); ++iter ){
		quantity = (*iter)->quantityIntersected(_depart,_arrivee,_light_radius);
		res += pow(quantity,2)*(3.0-2.0*quantity);
		if ( res >= 1.0 ){
			return 1.0;
		}
	}
	return res;	
}

Color Scene::illuminatedColor(const qglviewer::Vec& eye, Hit& hit) const{
	Color couleur;
	float lightQuantity;
	QList<Light*>::const_iterator iter;
	for (iter = this->lights_.constBegin(); iter != this->lights_.constEnd(); ++iter ){
		lightQuantity = (*iter)->quantityReachedAt(hit.intersection(), this);
		if ( lightQuantity != 0.0 ){
			couleur = couleur + lightQuantity*(*iter)->illuminatedColor(eye,hit);
		}
	}
	return couleur;
}
