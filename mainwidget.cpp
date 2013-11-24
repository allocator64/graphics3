#include "mainwidget.h"

#include <QMouseEvent>

#include <cmath>
#include <locale.h>

MainWidget::MainWidget(QWidget *parent) :
    QGLWidget(parent),
    angularSpeed(0)
{
}

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
    const float delta = 0.2;
    const float alpha = 0.05;
    QVector3D direct(
                std::cos(camera_direct.y()) * std::sin(camera_direct.x()),
                std::sin(camera_direct.y()),
                -std::cos(camera_direct.y()) * std::cos(camera_direct.x()));
    direct.normalize();
    if (keys.count(Qt::Key_W)) {
        camera_pos += direct * delta;
    }

    if (keys.count(Qt::Key_S)) {
        camera_pos -= direct * delta;
    }

    if (keys.count(Qt::Key_D)) {
        QVector3D d = QVector3D::normal(direct, direct + QVector3D(0, 1, 0));
        camera_pos += d * delta;
    }

    if (keys.count(Qt::Key_A)) {
        QVector3D d = QVector3D::normal(direct, direct + QVector3D(0, 1, 0));
        camera_pos -= d * delta;
    }


    if (keys.count(Qt::Key_Up)) {
        float lim = static_cast<float>(M_PI / 2.0 - M_PI / 100.0);
        camera_direct.setY(std::min(std::max(camera_direct.y() + alpha, -lim), lim));
    }

    if (keys.count(Qt::Key_Down)) {
        float lim = static_cast<float>(M_PI / 2.0 - M_PI / 100.0);
        camera_direct.setY(std::min(std::max(camera_direct.y() - alpha, -lim), lim));
    }

    if (keys.count(Qt::Key_Left)) {
        camera_direct.setX(camera_direct.x() - alpha);
        if (camera_direct.x() < -M_PI)
            camera_direct += QVector2D(2*M_PI, 0);
        if (camera_direct.x() > M_PI)
            camera_direct -= QVector2D(2*M_PI, 0);
    }

    if (keys.count(Qt::Key_Right)) {
        camera_direct.setX(camera_direct.x() + alpha);
        if (camera_direct.x() < -M_PI)
            camera_direct += QVector2D(2*M_PI, 0);
        if (camera_direct.x() > M_PI)
            camera_direct -= QVector2D(2*M_PI, 0);
    }


    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Update scene
    }
    updateGL();
}
//! [1]

void MainWidget::initializeGL()
{
    initializeGLFunctions();
    qglClearColor(Qt::black);
    initShaders();
    initObjects();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]


    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

//! [3]
void MainWidget::initShaders()
{
    // Override system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compile vertex shader
    if (!programLight.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderLight.glsl"))
        close();

    // Compile fragment shader
    if (!programLight.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderLight.glsl"))
        close();

    // Link shader pipeline
    if (!programLight.link())
        close();

    // Compile vertex shader
    if (!programDark.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderDark.glsl"))
        close();

    // Compile fragment shader
    if (!programDark.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderDark.glsl"))
        close();

    // Link shader pipeline
    if (!programDark.link())
        close();


    // if (!programLight.bind())
    //     close();

    // Restore system locale
    setlocale(LC_ALL, "");
}
//! [3]

//! [4]
void MainWidget::initObjects()
{
    // Load cube.png image
    glEnable(GL_TEXTURE_2D);
//    cube_texture = bindTexture(QImage(":/cube.png"));
    one_sphere.reset(new SphereEngine(2, 30)); //QImage(":/Earth.png"), *this));
    one_sphere->init(QImage(":/earth"), *this);
    one_sphere->_position = QVector3D(10, 0, 0);

    theSun.reset(new SphereEngine(5, 30)); //QImage(":/Earth.png"), *this));
    theSun->init(QImage(":/sun"), *this);
    theSun->_position = QVector3D(0, 0, 0);

    // qDebug() << QImage(":/Earth.png");
}

void MainWidget::keyPressEvent(QKeyEvent *key)
{
    keys.insert(key->key());
}

void MainWidget::keyReleaseEvent(QKeyEvent *key)
{
    keys.erase(key->key());
}

void MainWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 0.0001, zFar = 1000000.0, fov = 45.0;

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

    QMatrix4x4 shift_camera;
    shift_camera.rotate(-camera_direct.y() / M_PI * 180, 1, 0, 0);
    shift_camera.rotate(camera_direct.x() / M_PI * 180, 0, 1, 0);

    QMatrix4x4 projectionMatrix = projection * shift_camera;

    programDark.bind();
    theSun->draw(programDark, projectionMatrix, camera_pos);

    programLight.bind();
    programLight.setUniformValue("eyePos", QVector4D(camera_pos, 0));
    programLight.setUniformValue("lightPos", QVector4D(-camera_pos, 1));

    // Draw cube geometry
    one_sphere->draw(programLight, projectionMatrix, camera_pos);
}
