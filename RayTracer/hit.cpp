/**********************************************************************
 * \file hit.cpp
 *
 * \author Konubinix  (konubinix@gmail.com)
 * \date sam. 15:32:45 23/01/2010
 ***********************************************************************/
#include "hit.h"

void Hit::draw(){
 		glPointSize(10);
		glColor3f(0,0,1.0);
		glBegin(GL_LINES);
		glVertex3f(intersection_.x, intersection_.y, intersection_.z);
		glVertex3f(intersection_.x+normal_.x, intersection_.y+normal_.y, intersection_.z+normal_.z);
		glEnd();
}

qglviewer::Vec Hit::bump()const{
	Color __perturbation = perturbation();
	qglviewer::Vec __nouvelle_normale = normal_;
	__nouvelle_normale.x = __nouvelle_normale.x * (1+__perturbation.r);
	__nouvelle_normale.y = __nouvelle_normale.y * (1+__perturbation.g);
	__nouvelle_normale.z = __nouvelle_normale.z * (1+__perturbation.b);
	__nouvelle_normale.unit();
	return __nouvelle_normale;
}
