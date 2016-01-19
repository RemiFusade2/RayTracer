#include "viewer.h"
#include "scene.h"
#include "sphere.h"
#include <qfiledialog.h>
#include <QKeyEvent>
#include <math.h>

using namespace std;
using namespace qglviewer;

Viewer::~Viewer()
{
	delete scene_;
}

void Viewer::draw()
{
	// ////////////////////////////////////////
	// Trace la scene
	// ////////////////////////////////////////
	scene()->draw();
#ifdef DEBUG_SEGMENT
	Segment seg(ray_,hit_);
	seg.draw();
#endif
	rayTracer_.draw();


#ifdef KONIX_CONSTANT_PREVIEW
	if ( draw_counter_ > 50 ){
		draw_counter_ = 0;
		scene()->camera().frame().setPosition(camera()->position());
		scene()->camera().frame().setOrientation(camera()->orientation());
		rayTracer_.renderImage(true);
		scene()->camera().frame().setPosition(camera()->keyFrameInterpolator(1)->keyFrame(0).position());
		scene()->camera().frame().setOrientation(camera()->keyFrameInterpolator(1)->keyFrame(0).orientation());
	}
	draw_counter_++;
#endif
}

void Viewer::init()
{
#ifdef KONIX_CONSTANT_PREVIEW
	draw_counter_ = 0;
#endif
	// Key description for help window (press 'H')
	setKeyDescription(Qt::Key_F, "Erase current scene");
	setKeyDescription(Qt::Key_L, "Loads a new scene over the current one");
	setKeyDescription(Qt::Key_N, "Loads a new scene and replace the current one (=F + L)");
	setKeyDescription(Qt::Key_R, "Reloads the scene from the file");
	setKeyDescription(Qt::Key_S, "Shoot rays in the scene and saves the result");
	setKeyDescription(Qt::SHIFT+Qt::Key_S, "Shoot rays from <i>current</i> view point");
	setKeyDescription(Qt::SHIFT+Qt::Key_F5, "Shoot rays in the scene and show the result in a preview box");
	setKeyDescription(Qt::Key_F5, "Shoot rays from <i>current</i> view point and render the result in the preview box");
	setKeyDescription(Qt::Key_C, "Affiche sur la sortie standard la position et l'orientation de la caméra");

	// ################################################################################
	// INITIALISATION
	// ################################################################################
	// création de la scène
	setScene(new Scene());
	rayTracer_.setScene(scene());

	// So that transparent materials are correctly displayed.
	// Disable if rendering is too slow.
	glEnable(GL_BLEND);
#ifdef KONIX_LIGHTING
	glEnable(GL_LIGHTING);
#else
	glDisable(GL_LIGHTING);
#endif
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Restore previous viewer state (camera, key frames...)
	restoreStateFromFile();

	// Loads scene (prevents from pressing 'L' at each start).
	loadScene(sceneName_);

	// Set Camera to scene Camera. Set scene center and radius.
	initFromScene();

}

QString Viewer::helpString() const
{
	QString text("<h2>L a n c e r   d e   r a y o n s</h2>");
	text += "Ajoutez ici toute aide utile à l'utilisateur. ";
	text += "N'oubliez pas de commenter vos raccourcis clavier avec setKeyDescription().<br>";
	text += "Appuyez sur <b>Escape</b> pour quitter.";
	return text;
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_L :
#if QT_VERSION < 0x040000
		loadScene(QFileDialog::getOpenFileName("", "Scenes (*.scn);;All files (*)", this));
#else
		loadScene(QFileDialog::getOpenFileName(this, "Select a scene", ".", "Scenes (*.scn);;All files (*)"));
#endif
		this->initFromScene();
		break;
	case Qt::Key_F :
		delete scene_;
		setScene(new Scene());
		rayTracer_.setScene(scene());
		this->displayMessage("Scene erased");
		break;
	case Qt::Key_C :
		std::cout << scene_->camera() << std::endl;
		break;
	case Qt::Key_N :
		delete scene_;
		setScene(new Scene());
		rayTracer_.setScene(scene());
#if QT_VERSION < 0x040000
		loadScene(QFileDialog::getOpenFileName("", "Scenes (*.scn);;All files (*)", this));
#else
		loadScene(QFileDialog::getOpenFileName(this, "Select a scene", ".", "Scenes (*.scn);;All files (*)"));
#endif
		this->initFromScene();
		break;
	case Qt::Key_R :
		delete scene_;
		setScene(new Scene());
		rayTracer_.setScene(scene());
		this->loadScene(sceneName_);
		this->initFromScene();
		this->displayMessage("Scene reloaded");
		break;
	case Qt::Key_S :
#ifdef DEBUG_INTERSEC
		std::cout << "DEBUG_INTERSECT activé, on ne rend pas l'image car ça prend 3 plombe en IO" << std::endl;
		break;
#endif

#if QT_VERSION < 0x040000
		if ((e->state() == Qt::NoButton) || (e->state() == Qt::ShiftButton))
#else
			if ((e->modifiers() == Qt::NoModifier) || (e->modifiers() == Qt::ShiftModifier))
#endif	
			{
#if QT_VERSION < 0x040000
				if (e->state() == Qt::ShiftButton)
#else
					if (e->modifiers() == Qt::ShiftModifier)
#endif
					{
						// Shift+S renders image from current view point
						scene()->camera().frame().setPosition(camera()->position());
						scene()->camera().frame().setOrientation(camera()->orientation());
					}
	  
				rayTracer().renderImage();
				// Remplacer cette ligne par : const QString name = "result.jpg";
				// pour ne pas avoir à choisir un nom à chaque fois.
				const QString name = QFileDialog::getSaveFileName();
				rayTracer().saveImage(name);
	  
#if QT_VERSION < 0x040000
				if ((e->state() == Qt::ShiftButton) && (camera()->keyFrameInterpolator(1)))
#else
					if ((e->modifiers() == Qt::ShiftModifier) && (camera()->keyFrameInterpolator(1)))
#endif
					{
						// Restore initial scene camera
						scene()->camera().frame().setPosition(camera()->keyFrameInterpolator(1)->keyFrame(0).position());
						scene()->camera().frame().setOrientation(camera()->keyFrameInterpolator(1)->keyFrame(0).orientation());
					}
				break;
			}
	case Qt::Key_F5 :
#ifdef DEBUG_INTERSEC
		std::cout << "DEBUG_INTERSECT activé, on ne rend pas l'image car ça prend 3 plombe en IO" << std::endl;
		break;
#endif

#if QT_VERSION < 0x040000
		if ((e->state() == Qt::NoButton) || (e->state() == Qt::ShiftButton))
#else
			if ((e->modifiers() == Qt::NoModifier) || (e->modifiers() == Qt::ShiftModifier))
#endif	
			{
#if QT_VERSION < 0x040000
				if (e->state() != Qt::ShiftButton)
#else
					if (e->modifiers() != Qt::ShiftModifier)
#endif
					{
						// Shift+S renders image from current view point
						scene()->camera().frame().setPosition(camera()->position());
						scene()->camera().frame().setOrientation(camera()->orientation());
					}
	  
				rayTracer().renderImage(true);
				this->displayMessage("Preview computed");
#if QT_VERSION < 0x040000
				if ((e->state() != Qt::ShiftButton) && (camera()->keyFrameInterpolator(1)))
#else
					if ((e->modifiers() != Qt::ShiftModifier) && (camera()->keyFrameInterpolator(1)))
#endif
					{
						// Restore initial scene camera
						scene()->camera().frame().setPosition(camera()->keyFrameInterpolator(1)->keyFrame(0).position());
						scene()->camera().frame().setOrientation(camera()->keyFrameInterpolator(1)->keyFrame(0).orientation());
					}
				break;
			}
	default :
		QGLViewer::keyPressEvent(e);
	}
}

void Viewer::loadScene(const QString& name)
{
	if (name.isEmpty())
		return;

	scene()->loadFromFile(name);
	sceneName_ = QString(name);
 
	// Change QGLViewer settings according to scene
	setSceneCenter(scene()->center());
	setSceneRadius(scene()->radius(sceneCenter()));
}

// Make the first keyFrameInterpolator (binded to F1) contain one position, that cooresponds to the
// scene's camera position. Attention, camera() is moved to camera position.
void Viewer::initFromScene()
{
	camera()->setPosition(scene()->camera().frame().position());
	camera()->setOrientation(scene()->camera().frame().orientation());
	camera()->setFieldOfView(scene()->camera().fieldOfView());
  
	// Remove previous keyFrames in path 1 (if any)
	if (camera()->keyFrameInterpolator(1))
		camera()->keyFrameInterpolator(1)->deletePath();

	// Add current (i.e. scene camera) position to F1.
	camera()->addKeyFrameToPath(1);
}

void Viewer::select(const QPoint& point)
{
#ifdef DEBUG_INTERSEC
	std::cout << "#################### DEBUT calcul d'intersection avec la scène ####################" << std::endl;
#endif

	Vec orig,dir;
	camera()->convertClickToLine(point,orig,dir);
	// risque de perte de orig et dir quand on sort de la fonction
	Ray ray(orig,dir);
	rayTracer_.rayColor(ray,true,true);
#ifdef DEBUG_INTERSEC
	std::cout << "#################### FIN calcul d'intersection avec la scène ####################" << std::endl;
#endif

#ifdef DEBUG_RAYCOLOR
	std::cout << "#################### DEBUT du calcul de la couleur ####################" << std::endl;
	Color color = rayTracer_.rayColor(ray,true,false,1);
	std::cout << "(r,g,b) = (" << color.r << "," << color.g << "," << color.b << ")" << std::endl;
	std::cout << "#################### FIN calcul couleur ####################" << std::endl;
#endif

}
