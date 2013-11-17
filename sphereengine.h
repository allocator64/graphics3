#ifndef SPHEREENGINE_H
#define SPHEREENGINE_H

#include <QGLFunctions>
#include <QGLShaderProgram>

class SphereEngine : protected QGLFunctions
{
public:
    SphereEngine();
    virtual ~SphereEngine();

    void init();
    void drawSphereGeometry(QGLShaderProgram *program);

private:
    void initSphereGeometry();

    GLuint vboIds[2];
    size_t indices_size;
};

#endif // GEOMETRYENGINE_H
