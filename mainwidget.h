#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGLWidget>
#include <QGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QGLShaderProgram>

#include <unordered_set>

#include "engine.h"

class CubeEngine;

class MainWidget : public QGLWidget, protected QGLFunctions
{
	Q_OBJECT

public:
	explicit MainWidget(QWidget *parent = 0);
	~MainWidget();

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void timerEvent(QTimerEvent *e);

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void initShaders();
	void initObjects();

	void keyPressEvent(QKeyEvent *key);
	void keyReleaseEvent(QKeyEvent *key);

private:
	QBasicTimer timer;
	QGLShaderProgram programLight;
	QGLShaderProgram programDark;
    std::unique_ptr<SphereEngine> one_sphere;
    std::unique_ptr<SphereEngine> theSun;

	GLuint cube_texture;
	GLuint sphere_texture;

	std::unordered_set<int> keys;

	QMatrix4x4 projection;

	QVector2D mousePressPosition;
	QVector3D rotationAxis;
	qreal angularSpeed;
	QQuaternion rotation;

	QVector3D camera_pos;
	QVector2D camera_direct;
};

#endif // MAINWIDGET_H
