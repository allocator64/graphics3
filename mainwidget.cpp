#include "mainwidget.h"

#include <QMouseEvent>

#include <math.h>
#include <locale.h>

MainWidget::MainWidget(QWidget *parent) :
    QGLWidget(parent),
    angularSpeed(0)
{
}
*
MainWidget::~MainWidget()
{
    deleteTexture(cube_texture);
    deleteTexture(sphere_texture);
}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}
//! [0]

//! [1]
void MainWidget::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Update scene
        updateGL();
    }
}
//! [1]

void MainWidget::initializeGL()
{
    initializeGLFunctions();
    qglClearColor(Qt::black);
    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

//    one_cube.init();
    one_sphere.init();

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);

    camera_pos = QVector3D(0, 0, 0);
    camera_direct = QVector2D(0, 0);
}

//! [3]
void MainWidget::initShaders()
{
    // Override system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();

    // Restore system locale
    setlocale(LC_ALL, "");
}
//! [3]

//! [4]
void MainWidget::initTextures()
{
    // Load cube.png image
    glEnable(GL_TEXTURE_2D);
//    cube_texture = bindTexture(QImage(":/cube.png"));
    cube_texture = bindTexture(QImage(":/Earth.png"));

    // Set nearest filtering mode for texture minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set bilinear filtering mode for texture magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
//! [4]

//! [5]
void MainWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -4);
    matrix.rotate(rotation);

    QMatrix4x4 matrix_normal;
    matrix_normal.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("projection_matrix", projection);
    program.setUniformValue("model_view_matrix", matrix);
    program.setUniformValue("normal_matrix", matrix_normal);

    // Use texture unit 0 which contains cube.png
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, sphere_texture);
    program.setUniformValue("texture", 0);

    program.setUniformValue("eyePos", QVector4D(0, 0, 0, 1.0));
    program.setUniformValue("lightPos", QVector4D(5.0, 0, 0, 1.0));

    // Draw cube geometry
//    one_cube.drawCubeGeometry(&program);
    one_sphere.drawSphereGeometry(&program);
}
