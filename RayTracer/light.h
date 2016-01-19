#ifndef LIGHT_H
#define LIGHT_H

#include "color.h"
#include "hit.h"

class Scene;

class Light
{
public :
	Color color() const { return color_; }
	void setColor(const Color& color) { color_ = color; }

	virtual ~Light(){};

	virtual void initFromDOMElement(const QDomElement& e);
	virtual Color illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const;

	virtual void draw()const{};
	
	/**
	 * \brief Determine la quantité de lumière qui atteint le point pos
	 * \param pos : Position à laquelle on cherche à connaître la quantité de lumière
	 * \param scene : Utilisé pour faire des calculs d'intersection
	 * \return La quantité de lumière qui atteint pos, entre 0 et 1
	 */
	virtual float quantityReachedAt(const qglviewer::Vec& pos, const Scene* const scene)=0;
	void printColor(std::ostream& o) const {
		o << "Couleur : (" << color().r << "," << color().g << "," << color().b << ")" << std::endl;
	}
private:
	Color color_;
};

class AmbientLight : public Light
{
public :
	virtual Color illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const;
	void initFromDOMElement(const QDomElement& e);
	virtual float quantityReachedAt(const qglviewer::Vec& pos, const Scene* const scene){(void)pos;(void)scene;return 1.0;}
};

class DirectionalLight : public Light
{
public :
	virtual Color illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const;
	DirectionalLight();

	qglviewer::Vec direction() const { return direction_; }
	void setDirection(const qglviewer::Vec& direction) { direction_ = direction; direction_.normalize(); }

	virtual void initFromDOMElement(const QDomElement& e);
	virtual float quantityReachedAt(const qglviewer::Vec& pos, const Scene* const scene);
private:
	qglviewer::Vec direction_;
};

class PointLight : public Light
{
public :
	virtual Color illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const;
	qglviewer::Vec position() const { return position_; }
	float radius() const { return radius_; }
	float getA() const { return a; }
	float getB() const { return b; }
	float getC() const { return c; }

	void setPosition(const qglviewer::Vec& position) { position_ = position; }
	void setRadius(const float radius) { radius_ = radius; }
	void setA(const float a) { this->a = a;}
	void setB(const float b) { this->b = b;}
	void setC(const float c) { this->c = c;}


	virtual void initFromDOMElement(const QDomElement& e);
	virtual float quantityReachedAt(const qglviewer::Vec& pos, const Scene* const scene);
	virtual void draw()const;

private:
	/**
	 * \brief Fonction privé qui indique si une partie de la
	 * lumière est visible depuis la position pos
	 
	 * La fonction utilise une base u,v sur le plan
	 * perpendiculaire à l'axe pos -> lumière. Les coordonnées x,y
	 * servent à échantilloner le carré représentant la lumière,
	 * (x,y) \in (-radius_/2,..,radius_/2)

	 * \param pos : position à partir de laquelle on regarde la lumière
	 * \param scene : pour les calcul d'intersection
	 * \param u,v : les vecteurs (unitaires) de base du plans perpendiculaire à l'axe pos->lumière
	 * \param x,y : les coordonnées sur (u,v) de l'échantillon de lumière considéré
	 */
	bool visibleFrom(const qglviewer::Vec& pos, const Scene* const scene, 
			 qglviewer::Vec& u, qglviewer::Vec& v,
			 float x, float y);
	qglviewer::Vec position_;
	float radius_;
	float a;
	float b;
	float c;
};

std::ostream& operator<<(std::ostream& o, const AmbientLight& al);
std::ostream& operator<<(std::ostream& o, const DirectionalLight& dl);
std::ostream& operator<<(std::ostream& o, const PointLight& pl);

#endif // LIGHT_H
