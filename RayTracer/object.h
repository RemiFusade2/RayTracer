#ifndef OBJECT_H
#define OBJECT_H

#include "QGLViewer/qglviewer.h"
#include "material.h"
#include "hit.h"
#include "ray.h"

// Classe générique abstraite dont vont deriver les objets de la scene.
class Object
{
public :
	virtual ~Object() {};
  
	// Accesseurs
	const Material& material() const { return material_; }
	const qglviewer::Frame& frame() const { return frame_; }

	// Modifieurs
	void setMaterial(const Material& material) { material_ = material; }
	void setFrame(const qglviewer::Frame& frame) { frame_ = frame; }

	// Affichage openGL de l'objet
	virtual void draw() const=0;

	void drawAxis()const;
	
	void traceRepaire()const;

	// Rayon d'une sphère englobante (meme grossiere).
	virtual float boundingRadius() const = 0;

	// Methode principale d'intersection entre l'objet et un rayon
	virtual bool intersect(const Ray&, Hit& hit) const = 0;

	/** \brief Détermine la quantité de rayon entre le vecteur
	 * _depart et le vecteur _arrivee qui est filtré par l'objet

	 * En pratique, on vérifie que l'objet intersecte le
	 * segment. On dit : si l'objet intersecte le rayon, il filtre
	 * 1.0, si l'objet, grossit un peu,n'intersecte rien, il
	 * filtre 0.0, sinon, la portion de rayon qui passe entre
	 * l'objet "vu en gros" et l'objet initial détermine à quel
	 * point il fait pénombre.
	 
	 * \param _depart : Point de départ du segment
	 * \param _arrivee : Point d'arrivé du segment

	 * \return Une valeur entre 0 et 1, 0 veut dire n'intersecte
	 * pas alors que 1 veut dire intersecte totalement (ie
	 * intersection même dans le cas d'un intersect classique). La
	 * valeur renvoyée doit être continue et si possible C^2 (pour
	 * ne pas voir les différences)
	 */
	virtual float quantityIntersected(const qglviewer::Vec& _depart, const qglviewer::Vec& _arrivee,float _light_radius) const;

	virtual void initFromDOMElement(const QDomElement& e);

protected :
	// Tous les objets ont un repere et un materiau.
	qglviewer::Frame frame_;
	Material material_;
};

#endif // OBJECT_H
