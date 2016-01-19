#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include "rayTracer.h"
#include "ray.h"
#include "hit.h"
class Scene;

class Viewer : public QGLViewer
{
public:
	~Viewer();
  
	void loadScene(const QString& name);

	Scene* scene() const { return scene_; };
	const RayTracer& rayTracer() const { return rayTracer_; }

	void setScene(Scene* const scene) { scene_ = scene; };
	void setRayTracer(const RayTracer& rayTracer) { rayTracer_ = rayTracer; }

	void setSceneName(QString name){ sceneName_ = name; }

protected :
	virtual void draw();
	virtual void init();
	virtual QString helpString() const;
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void select(const QPoint& point);

private:
	// Definit la position F1 comme étant celle de la camera.
	void initFromScene();

	Scene* scene_;
	RayTracer rayTracer_;

	QString sceneName_;

#ifdef KONIX_CONSTANT_PREVIEW
	int draw_counter_;
#endif
};

#endif // VIEWER_H
