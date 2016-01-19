#include "ray.h"
#include <qmessagebox.h>

using namespace qglviewer;

Ray::Ray()
{
  setStart(Vec());
  setDirection(Vec(0.0, 0.0, 1.0));
#ifdef DEBUG_RAYONS
	std::cout << "Création d'un rayon de " << start_ << " vers " << start_+direction_ << std::endl;  
#endif
}

Ray::Ray(const Vec start, const Vec direction)
{
  setStart(start);
  setDirection(direction);
#ifdef DEBUG_RAYONS
	std::cout << "Création d'un rayon de " << start_ << " vers " << start_+direction_ << std::endl;  
#endif

}

void Ray::setDirection(const Vec& direction)
{
#ifdef DEBUG_RAYONS
	std::cout << "Changement de la direction du rayon de " << direction_ << " à "<< direction << std::endl;
#endif
  if (direction.norm() < 1e-8)
    {
      QMessageBox::critical(NULL, "Ray::setDirection error", "Null length direction in Ray::setDirection");
      return;
    }
  direction_ = direction;
  direction_.normalize();
}

void Ray::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0,1,0);
	glLineWidth (2.0);
	glPushMatrix();
	
	glBegin(GL_LINES); 
	glVertex3f(start_[0],start_[1],start_[2]);
	glVertex3f((start_+10*direction_)[0],
		   (start_+10*direction_)[1],
		   (start_+10*direction_)[2]);
	glEnd();

	glPopMatrix();

}

std::ostream& operator<<(std::ostream& o, const Ray& ray){
	o << "start : " << ray.start() << std::endl;
	o << "dir   : " << ray.direction() << std::endl;
	return o;
}
