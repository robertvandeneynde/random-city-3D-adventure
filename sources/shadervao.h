#ifndef SHADERVAO_H
#define SHADERVAO_H

#include <QGLWidget>

//#include <QGLFunctions> // old Qt4
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class ShaderVAO : public QOpenGLFunctions
{
public:
    ShaderVAO();

    void dessiner();

public:
    QOpenGLShaderProgram prog;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;

    QMatrix4x4 matrice;
};

#endif // SHADERVAO_H
