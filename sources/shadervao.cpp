#include "shadervao.h"

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

ShaderVAO::ShaderVAO()
{
    initializeOpenGLFunctions();

    float vertices[] = {
        0.6, 0.6, 0.0,
        0.0, -0.6, 0.0,
        -0.6, 0.6, 0.0,
    };

    vbo.create();  // glGenBuffers(...);
    vbo.bind();  // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vbo.allocate(vertices, sizeof(vertices));  // glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW);

    prog.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/base.vert");
    prog.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/base.frag");

    vao.create();  // glGenVertexArrays(...)
    vao.bind(); // glBindVertexArray(vao);

    prog.bind();
    prog.enableAttributeArray("position");  // glEnableVertexAttribArray(VAO_position);
    prog.setAttributeBuffer("position", GL_FLOAT, 0, 3); // (offset, size, stride=0); // glVertexAttribPointer(VAO_position, 4, GL_FLOAT, False, 0, reinterpret_cast<const void *>(offset)(0)); (False,
    prog.release();
    vao.release();  // glBindVertexArray(0);
    vbo.release();  // glBindBuffer(GL_ARRAY_BUFFER, 0);

    matrice.translate({0.5, 0, 0});
}

// appelle glDrawArrays
void ShaderVAO::dessiner() {
    prog.bind();  // glUseProgram(shader_program);
    vao.bind();  // glBindVertexArray(vertex_array_object);

    prog.setUniformValue("matrice", matrice);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    vao.release();  // glBindVertexArray(0);
    prog.release();  // glUseProgram(0);
}
