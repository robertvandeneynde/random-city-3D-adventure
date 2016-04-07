#ifndef FONCTIONSGL_H
#define FONCTIONSGL_H

#include <QGLWidget>

#include "VertexArray.h"

void glBlocCreerSommets(VertexArray & sommets ,double x, double y, double dx, double dy, double hauteur);
void glBlocCreerSommets(VertexArray & sommets ,double x, double y, double z, double dx, double dy, double hauteur);

void glDrawRepere(int echelle);

#endif // FONCTIONSGL_H
