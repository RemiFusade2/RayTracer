#include "camera.h"
#include "scene.h"
#include <qmessagebox.h>
#include <QGLViewer/vec.h>

Camera::Camera()
{
	setXResolution(64);
	setYResolution(64);
	setFieldOfView(M_PI / 4.0);
}

void Camera::initFromDOMElement(const QDomElement& e)
{
	if (e.tagName() != "Camera")
	{
		QMessageBox::critical(NULL, "Camera init error", "Camera::initFromDOMElement, bad DOM tagName.\nExpecting 'Camera', got "+e.tagName());
		return;
	}

	if (!e.hasAttribute("fieldOfView"))
		QMessageBox::warning(NULL, "Camera error", "Camera has undefined fieldOfView. Using pi/4.");
	setFieldOfView(e.attribute("fieldOfView", "0.7854").toFloat());

	if (!e.hasAttribute("xResolution"))
		QMessageBox::warning(NULL, "Camera error", "Camera has undefined xResolution. Using 64.");
	setXResolution(e.attribute("xResolution", "64").toInt());

	if (!e.hasAttribute("yResolution"))
		QMessageBox::warning(NULL, "Camera error", "Camera has undefined yResolution. Using 64.");
	setYResolution(e.attribute("yResolution", "64").toInt());

#ifdef DEBUG
	sampling_ = 1;
#else
	sampling_ = e.attribute("sampling", "1").toInt();
#endif

	QDomNode n = e.firstChild();
	while (!n.isNull())
	{
		QDomElement e = n.toElement();
		if (!e.isNull())
		{
			if (e.tagName() == "Frame")
				frame_.initFromDOMElement(e);
			else
				QMessageBox::warning(NULL, "Camera child error", "Unsupported Camera child : "+e.tagName());
		}
		else
			QMessageBox::warning(NULL, "Camera XML error", "Error while parsing Camera XML document");
		n = n.nextSibling();
	}
#ifdef DEBUG
	std::cout << "Intialisation d'une caméra de sampling : "<< sampling_ << std::endl;
#endif
}

void Camera::setXResolution(int xResolution)
{
	if (xResolution <= 0)
	{
		QMessageBox::warning(NULL, "Camera error", "Negative image xResolution - Skipping");
		return;
	}
	xResolution_ = xResolution;
}

void Camera::setYResolution(int yResolution)
{
	if (yResolution <= 0)
	{
		QMessageBox::warning(NULL, "Camera error", "Negative image yResolution - Skipping");
		return;
	}
	yResolution_ = yResolution;
}

void Camera::draw(float radius) const
{
#ifdef KONIX_CAMERA
	float longueur = 0.2;
	float hauteur = longueur * sin(fieldOfView_);
	float largeur = hauteur * xResolution_ / yResolution_;
	float profondeur = longueur*-cos(fieldOfView_);
#endif
	glColor4f(0,0,0,1);
	glPointSize(0.1);
	glLineWidth(1.5);

	glPushMatrix();
	glMultMatrixd(frame().matrix());

	glScalef(radius,radius,radius);
#ifdef KONIX_CAMERA
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(largeur/2,hauteur/2,profondeur);
	glVertex3f(0,0,0);
	glVertex3f(-largeur/2,hauteur/2,profondeur);
	glVertex3f(largeur/2,hauteur/2,profondeur);
	glVertex3f(-largeur/2,hauteur/2,profondeur);

	glVertex3f(0,0,0);
	glVertex3f(largeur/2,-hauteur/2,profondeur);
	glVertex3f(0,0,0);
	glVertex3f(-largeur/2,-hauteur/2,profondeur);
	glVertex3f(largeur/2,-hauteur/2,profondeur);
	glVertex3f(-largeur/2,-hauteur/2,profondeur);

	glVertex3f(0,0,0);
	glVertex3f(-largeur/2,hauteur/2,profondeur);
	glVertex3f(0,0,0);
	glVertex3f(-largeur/2,-hauteur/2,profondeur);
	glVertex3f(-largeur/2,-hauteur/2,profondeur);
	glVertex3f(-largeur/2,hauteur/2,profondeur);

	glVertex3f(0,0,0);
	glVertex3f(largeur/2,hauteur/2,profondeur);
	glVertex3f(0,0,0);
	glVertex3f(largeur/2,-hauteur/2,profondeur);
	glVertex3f(largeur/2,-hauteur/2,profondeur);
	glVertex3f(largeur/2,hauteur/2,profondeur);
	glEnd();
#endif

#ifdef KONIX_GRILLE
	glScalef(2,2,2);
	glLineWidth(5);
	for ( int x = 0 ; x < xResolution_ ; ++x ){
		for ( int y = 0 ; y < yResolution_ ; ++y ){
			glColor4f(1,1,1,0.5);
			glBegin(GL_QUADS);
			glVertex3f(-largeur/2+x*(largeur/xResolution_),-hauteur/2+y*(hauteur/yResolution_),profondeur);
			glVertex3f(-largeur/2+(x+1)*(largeur/xResolution_),-hauteur/2+y*(hauteur/yResolution_),profondeur);
			glVertex3f(-largeur/2+(x+1)*(largeur/xResolution_),-hauteur/2+(y+1)*(hauteur/yResolution_),profondeur);
			glVertex3f(-largeur/2+x*(largeur/xResolution_),-hauteur/2+(y+1)*(hauteur/yResolution_),profondeur);
			glEnd();
			glColor4f(0,0,0,1);
			glBegin(GL_LINE_LOOP);
			glVertex3f(-largeur/2+x*(largeur/xResolution_),-hauteur/2+y*(hauteur/yResolution_),profondeur);
			glVertex3f(-largeur/2+(x+1)*(largeur/xResolution_),-hauteur/2+y*(hauteur/yResolution_),profondeur);
			glVertex3f(-largeur/2+(x+1)*(largeur/xResolution_),-hauteur/2+(y+1)*(hauteur/yResolution_),profondeur);			
			glVertex3f(-largeur/2+x*(largeur/xResolution_),-hauteur/2+(y+1)*(hauteur/yResolution_),profondeur);			
			glEnd();
			
#ifdef DEBUG_CAMERA
			float x_im = -largeur/2 + (largeur/xResolution_) * (x+.5);
			float y_im = hauteur/2 - (hauteur/yResolution_) * (y+.5);
			
			glPointSize(20);
			glColor3f(1,0,1);
			glBegin(GL_POINTS);
			glVertex3f(x_im,y_im,profondeur);
			glEnd();
#endif
		}
	}
#endif

	glPopMatrix();

#ifdef DEBUG_CAMERA_RAYONS
	Ray ray;
	for ( int x = 0 ; x < xResolution_ ; ++x ){
		for ( int y = 0 ; y < yResolution_ ; ++y ){
			for ( float m = (-0.5+1.0/(sampling_+1)) ; m < 0.5 ; m+= 1.0/(sampling_+1)){
				for ( float n = (-0.5+1.0/(sampling_+1)) ; n < 0.5 ; n+= 1.0/(sampling_+1)){
					ray = getPixelRay(x+m,y+n);
					ray.draw();
				}
			}
		}
	}
#endif

}

Ray Camera::getPixelRay(float x, float y) const
{
	// on prend une longueur de 1
	float hauteur = sin(fieldOfView_);
	float largeur = hauteur * xResolution_ / yResolution_;
	float profondeur = -cos(fieldOfView_);

	float x_im = -largeur/2 + (largeur/xResolution_) * (x+.5);
	float y_im = hauteur/2 - (hauteur/yResolution_) * (y+.5);
	
	qglviewer::Vec dir_im(x_im,y_im,profondeur);
	qglviewer::Vec dir_world = frame().inverseTransformOf(dir_im);
	qglviewer::Vec orig = frame().position();

#ifdef DEBUG_CAMERA
	glPointSize(20);
	glColor3f(1,0,1);
	glBegin(GL_POINTS);
	glVertex3f(orig.x,orig.y,orig.z);
	glEnd();
#endif
	return Ray(orig, dir_world);
}

void Camera::drawAllRays() const
{
	for (int i=0; i<xResolution(); ++i)
		for (int j=0; j<yResolution(); ++j)
			getPixelRay(i,j).draw();
}

std::ostream& operator<<(std::ostream& o, const Camera& camera){
	o << "position    : " << camera.frame().position() << std::endl;
	o << "orientation : " <<  camera.frame().orientation() << std::endl;
	return o;
}
