#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#ifdef DEBUG_INTERSEC
#define PROF_RAYONS 0.0001
#else
#define PROF_RAYONS 40
#endif

#include <qimage.h>
#include "color.h"
#include "ray.h"
#include "hit.h"

class Scene;

class Segment{
public:
	void draw()const;

	Segment(){}

	Segment(const Ray& ray, const Hit& hit) : depart_(ray.start()), arrive_(hit.intersection()) , normal_au_point_arrive_(hit.bump()) {}
	Segment(qglviewer::Vec depart, qglviewer::Vec arrive, qglviewer::Vec normal) : depart_(depart), arrive_(arrive) , normal_au_point_arrive_(normal) {}
private:
	qglviewer::Vec depart_;
	qglviewer::Vec arrive_;
	qglviewer::Vec normal_au_point_arrive_;
};


class RayTracer
{
public :
	RayTracer() : scene_(NULL) {};

	Scene* scene() { return scene_; }
	const QImage& image() const { return image_; }

	void draw()const;

	void setScene(Scene* scene) { scene_ = scene; }

	void renderImage(bool _preview=false) const;
	void saveImage(const QString& name, bool overwrite=false) const;

	Color rayColor(const Ray& ray,bool fromEye=true, bool createChemin=false, float prof=PROF_RAYONS) const;

private:
	Scene* scene_;
	mutable QImage image_;
	mutable QList<Segment*> cheminRayon_;
};

#endif // RAY_TRACER_H
