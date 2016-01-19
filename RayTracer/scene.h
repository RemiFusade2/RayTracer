#ifndef SCENE_H
#define SCENE_H

#include <QList>

#include "object.h"
#include "camera.h"
#include "light.h"

class Scene
{
public :
	void draw() const;
	void addObject(Object* o);
	void addLight(Light* l);
	
	/**
	 * Supprime les objets et les lampes de la scène
	 */
	~Scene();

	// ************************************************************
	// Parcours tous les noeuds de profondeur 1 dans le fichier
	// XML donnée et crée la scène à partir
	// d'eux ************************************************************
	void loadFromFile(const QString& filename);

	qglviewer::Vec center() const;
	float radius(const qglviewer::Vec& center) const;

	void setCamera(const Camera& camera) { camera_ = camera; }
	const Camera& camera() const { return camera_; }
	Camera& camera() { return camera_; }

	// Methode principale de calcul d'intersection dans la scene
	// entre les objets et un rayon
	bool intersect(const Ray&, Hit& hit) const;

	/** Methode principale de calcul de la quantité de rayon
	    lumineux qui ne traverse pas le segment _depart ->
	    _arrivee

	    \return 1.0 si rien ne traverse, 0.0 si tout
	    traverse ou une proportion de ce qui traverse
	*/
	float quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee,float _light_radius) const;

	Color backgroundColor() const { return backgroundColor_; }

	// fonction qui, à partir du hit (point d'inter du rayon et de
	// la surface) retourne la couleur associée en prenant en
	// compte les lumères
	Color illuminatedColor(const qglviewer::Vec& eye, Hit&) const;

private:
	void initElement(QDomElement& e);
	Camera camera_;
	QList<Object*> objects_;
	Color backgroundColor_;
	QList<Light*> lights_;
};

#endif // SCENE_H
