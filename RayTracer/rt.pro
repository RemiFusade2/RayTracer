TEMPLATE = app
TARGET   = rt
CONFIG  += qt opengl warn_on thread test debug

PROG_DEF_FLAGS = -Wall -Werror -W -Wextra -ansi 

#DEBUG = -DDEBUG -DDEBUG_INTERSEC -DDEBUG_CAMERA -DKONIX_GRILLE -DDEBUG_CAMERA_RAYONS -DKONIX_CAMERA -DDEBUG_HIT_NORMAL -DDEBUG_RAYONS -DDEBUG_PLAN  -DKONIX_LIGHTING -DDEBUG_RAYTRACER -DDEBUG_FRESNEL_REFLECTION -DDEBUG_SOFT_SHADOW -DDEBUG_LOCAL_AXIS  -DKONIX_CONSTANT_PREVIEW -DDEBUG_SPLINE

DEBUG = -DDEBUG -DKONIX_CAMERA -DDEBUG_RAYTRACER -DKONIX_LIGHTING

QMAKE_CFLAGS_RELEASE += $${PROG_DEF_FLAGS}
QMAKE_CXXFLAGS_RELEASE += $${PROG_DEF_FLAGS}

QMAKE_CFLAGS_DEBUG += -pg $${PROG_DEF_FLAGS} $${DEBUG}
QMAKE_CXXFLAGS_DEBUG += -pg $${PROG_DEF_FLAGS} $${DEBUG}

QMAKE_LFLAGS += $${PROG_DEF_FLAGS}
QMAKE_LFLAGS_DEBUG += -pg

HEADERS  = viewer.h   sphere.h   triangle.h   maillage.h	material.h   object.h   scene.h   ray.h   camera.h   light.h   rayTracer.h   hit.h color.h plane.h cylindre.h disque.h spline.h
SOURCES  = viewer.cpp sphere.cpp triangle.cpp maillage.cpp	material.cpp object.cpp scene.cpp ray.cpp camera.cpp light.cpp rayTracer.cpp main.cpp plane.cpp cylindre.cpp disque.cpp spline.cc hit.cpp

# Qt 2.3 users must edit config.pri
include( config.pri )


CONFIG(test){
;; TARGET = test_segment

;; HEADERS  = viewer.h   sphere.h   material.h   object.h   scene.h   ray.h   camera.h   light.h   rayTracer.h   hit.h color.h 
;; SOURCES  = viewer.cpp sphere.cpp material.cpp object.cpp scene.cpp ray.cpp camera.cpp light.cpp rayTracer.cpp test_segment.cpp
}
