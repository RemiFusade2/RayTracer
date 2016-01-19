#include "rayTracer.h"
#include "scene.h"
#include "camera.h"
#include "hit.h"
#include "ray.h"
#include "macros.h"

#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <math.h>

void RayTracer::renderImage(bool _preview) const
{
	float prevXRes,prevYRes, N;
	// Barre de progression
	QProgressDialog q;
	float pourcentageEffectue = 0;
	if ( _preview ){
		prevXRes = scene_->camera().xResolution();
		prevYRes = scene_->camera().yResolution();
		scene_->camera().setXResolution(100);
		scene_->camera().setYResolution(100);
		N = 1;
	}else{
		N = scene_->camera().sampling();
		q.setRange(0,scene_->camera().xResolution());
		q.setMinimumDuration(1);
		q.setValue(0);
		prevYRes = -1;
		prevXRes = -1;
	}

	QImage image(scene_->camera().xResolution(),
		     scene_->camera().yResolution(),
		     QImage::Format_ARGB32);

	Ray pixel_ray;
	Color normalisateur(1.0/(N*N),
			    1.0/(N*N),
			    1.0/(N*N));
	for ( float i = 0 ; i < scene_->camera().xResolution() ; i++){
		for ( float j = 0 ; j < scene_->camera().yResolution() ; j++ ){
			Color pixel_color;
			for ( float m = (-0.5+1.0/(N+1)) ; m < 0.5 ; m+= 1.0/(N+1)){
				for ( float n = (-0.5+1.0/(N+1)) ; n < 0.5 ; n+= 1.0/(N+1)){
					pixel_ray = scene_->camera().getPixelRay(i+m,j+n);
					pixel_color += rayColor(pixel_ray)*normalisateur;
				}
			}
			image.setPixel((int)i,(int)j,pixel_color);
		}
		if ( ! _preview ){
		  // ça c'est de la connerie, moi je trouve ça fun mais si ça vous plait pas vous pouvez l'enlever
		  // référence: les sims , au passage
		  pourcentageEffectue = (1.0*i)/scene_->camera().xResolution();
		  if (pourcentageEffectue < 0.2) {
		    q.setLabelText("Initialisation de l'entropie de premiere approximation...");
		  } else if (pourcentageEffectue < 0.4) {
		    q.setLabelText("Envoi des rayons de la mort...");
		  } else if (pourcentageEffectue < 0.6) {
		    q.setLabelText("Simulation du comportement social...");
		  } else if (pourcentageEffectue < 0.8) {
		    q.setLabelText("Ajout de granularite...");
		  } else {
		    q.setLabelText("Lissage des incoherences...");
		  }
		  
		  // ça c'est au cas où on ajouter un bouton "annuler" sur la progressBar
		  // Il me semble que c'est possible de le faire facilement
		  if (q.wasCanceled ()) {
		    break;
		  }
		  // ça par contre c'est indispensable
		  q.setValue( i );
		}
	}
	image_ = image;
	if ( _preview ){
		scene_->camera().setXResolution(prevXRes);
		scene_->camera().setYResolution(prevYRes);
	}
}

Color RayTracer::rayColor(const Ray& ray, bool fromEye, bool createChemin, float prof) const
{
	if ( prof <= 0 ){
		return Color();
	}

	if ( fromEye && createChemin ){
		// Premier rayon, on flush la liste de segments
		QList<Segment*>::const_iterator iter;
		for (iter = this->cheminRayon_.constBegin(); iter != this->cheminRayon_.constEnd(); ++iter ){
			delete (*iter);
		}
		cheminRayon_.clear();
	}

	Hit hit;
	if ( scene_->intersect(ray,hit) ){
#ifdef DEBUG_HIT_NORMAL
		Color color(hit.bump().x*0.5+0.5,
			    hit.bump().y*0.5+0.5,
			    hit.bump().z*0.5+0.5);
		return color;
#else
#ifdef DEBUG_SOFT_SHADOW
		if ( fromEye ){
			std::cout << "Quantité intersectée : " << scene_->quantityIntersected(ray.start(),hit.intersection(),0.5) << std::endl;
		}
#endif
		qglviewer::Vec normal_ = hit.bump();
		Color couleur;
		if ( createChemin ){
			cheminRayon_.append(new Segment(ray,hit));
		}else{
			couleur = ( ((RayTracer*)this)->scene()->illuminatedColor(ray.start(),hit) );
		}
		// creation du rayon réfléchi
		qglviewer::Vec L = -ray.direction();
		float prod_scal = L.x*normal_.x+L.y*normal_.y+L.z*normal_.z;
		Ray reflec(hit.intersection(),
			   (2*prod_scal*normal_-L).unit()
			);
		if ( createChemin ){
			cheminRayon_.append(new Segment(reflec.start(),reflec.start()+reflec.direction(),qglviewer::Vec(0,0,0)));
		}
		// Prise en compte du rayon reflechi
 		Color reflec_couleur = rayColor(reflec,false,createChemin,prof-hit.time());
		// Prise en compte de la réflexion de Fresnel avec
		// l'approximation de Shlick, le coefficient de
		// réflexion est pondéré par un cos puissance 5 cf
		// http://en.wikipedia.org/wiki/Schlick%27s_approximation
		Color reflectiveColor = hit.material().reflectiveColor();
		Color un_moins_reflectiveColor(1.0-reflectiveColor.r,
					       1.0-reflectiveColor.g,
					       1.0-reflectiveColor.b);
		float tmp = hit.normal().x*L.x+hit.normal().y*L.y+hit.normal().z*L.z; // Evite que le ABS foire
		float un_moins_cosTheta_puissance_cinq = pow((1-(ABS(tmp))),5);
#ifdef DEBUG_FRESNEL_REFLECTION
		std::cout << "(1-cos(theta))^5 : " << un_moins_cosTheta_puissance_cinq << std::endl;
		std::cout << "R0 : (" << reflectiveColor.r << "," << reflectiveColor.g << "," << reflectiveColor.b << ")" << std::endl;
		std::cout << "1-R0 : (" << un_moins_reflectiveColor.r << "," << un_moins_reflectiveColor.g << "," << un_moins_reflectiveColor.b << ")" << std::endl;
		std::cout << "Couleur reflechie avant Fresnel : (" << reflec_couleur.r << "," << reflec_couleur.g << "," << reflec_couleur.b << ")" << std::endl;
#endif
		reflec_couleur = reflec_couleur * (reflectiveColor + un_moins_cosTheta_puissance_cinq*un_moins_reflectiveColor);
#ifdef DEBUG_FRESNEL_REFLECTION
		std::cout << "Couleur reflechie, après Fresnel : (" << reflec_couleur.r << "," << reflec_couleur.g << "," << reflec_couleur.b << ")" << std::endl;
#endif
		return couleur+reflec_couleur;
#endif
	}else{
		if ( createChemin ){
			cheminRayon_.append(new Segment(ray.start(), ray.start()+10*ray.direction(),qglviewer::Vec()));
		}
		
		if ( fromEye ){
			return scene_->backgroundColor();
		}else{
			return Color();
		}
	}
}

void RayTracer::saveImage(const QString& name, bool overwrite) const
{
	QString fileName = name;
	QFileInfo info(fileName);

	// Append jpg extension if needed
#if QT_VERSION < 0x040000
	if (info.extension(false) != "jpg")
#else
		if (info.suffix() != "jpg")
#endif
		{
			fileName += ".jpg";
			info.setFile(fileName);
		}

	// Prevent overwriting an existing file
	if (info.exists() && !overwrite)
		if (QMessageBox::warning(NULL, "Overwrite file ?",
					 "File "+info.fileName()+" already exists.\nOverwrite ?",
					 QMessageBox::Yes,
					 QMessageBox::Cancel) == QMessageBox::Cancel)
			return;

	if (!image().save(fileName, "JPEG", 95))
		QMessageBox::information(NULL, "Error", "Error while saving "+fileName);
	else
		QMessageBox::information(NULL, "Saving done", "Image successfully saved in "+fileName);
}

void RayTracer::draw()const{
#ifdef DEBUG_RAYTRACER
	QList<Segment*>::const_iterator iter;
	for (iter = this->cheminRayon_.constBegin(); iter != this->cheminRayon_.constEnd(); ++iter ){
		(*iter)->draw();
	}
#endif
	if ( image_.height() != 0 ){
		// Il y a une image stockée
		QImage gldata = QGLWidget::convertToGLFormat(image_.scaled(100,100));
		glDrawPixels(gldata.width(), gldata.height(), GL_RGBA, GL_UNSIGNED_BYTE, gldata.bits());
	}
}

void Segment::draw() const{
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.6,0.6,0.6);
	glLineWidth (2.0);
	glPushMatrix();
	
	glBegin(GL_LINES); 
	glVertex3f(depart_[0],depart_[1],depart_[2]);
	glVertex3f(arrive_[0],arrive_[1],arrive_[2]);
	glEnd();

	if ( normal_au_point_arrive_.norm() != 0 ){
		glColor3f(0.2,0.2,0);
		glLineWidth (5.0);
		glBegin(GL_LINES); 
		glVertex3f(arrive_[0],arrive_[1],arrive_[2]);
		glVertex3f((arrive_+normal_au_point_arrive_)[0],(arrive_+ normal_au_point_arrive_)[1],(arrive_+ normal_au_point_arrive_)[2]);
		glEnd();
	}

	glPopMatrix();

}
