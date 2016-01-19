#ifndef RAY_H
#define RAY_H

#include <QGLViewer/frame.h>

class Ray
{
public :
	Ray();
	Ray(const qglviewer::Vec start, const qglviewer::Vec dir);
  
	qglviewer::Vec start() const { return start_; }
	qglviewer::Vec direction() const { return direction_; }
  
	void setStart(const qglviewer::Vec& start) {
#ifdef DEBUG_RAY
		std::cout << "Changement du start du rayon de " << start_ << " à "<< start << std::endl;
#endif
		start_ = start; }

	void setDirection(const qglviewer::Vec& direction);

	void draw() const;


private:
	qglviewer::Vec start_, direction_;
};

std::ostream& operator<<(std::ostream& o, const Ray&);

#endif // RAY_H
