#include "viewer.h"
#include <qapplication.h>

int main(int argc, char** argv)
{
#ifdef DEBUG
	std::cout << "############################## MODE DEBUG ##############################" << std::endl;
#endif

  // Read command lines arguments.
  QApplication application(argc,argv);

  // Instantiate the viewer.
  Viewer v;
  if ( argc > 1 ){
	  v.setSceneName(application.arguments().at(1));
  }else{
	  v.setSceneName(QString("troisSpheres.scn"));
  }

  // Make the viewer window visible on screen.
  v.show();
  
#if QT_VERSION < 0x040000
  // Set the viewer as the application main widget.
  application.setMainWidget(&v);
#endif
    
  // Run main loop.
  return application.exec();
}
