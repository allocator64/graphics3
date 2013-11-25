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

class MainWidget : public QGLWidget, protected QGLFunctions
{
	Q_OBJECT

public:
	explicit MainWidget(QWidget *parent = 0);
	
protected:
	void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
	void timerEvent(QTimerEvent *e);

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void initShaders();
	void initObjects();
	void bindTextureWrap(GLuint textureIdx);

	void keyPressEvent(QKeyEvent *key);
	void keyReleaseEvent(QKeyEvent *key);

	void viewUp(float alpha);
	void viewRight(float alpha);
	void viewForward(float delta);
	void modifyAngle(float alpha);
	void changeDeltaTime(float delta);
	QVector3D getDirection();
	
private:
	QBasicTimer timer;
	
	QGLShaderProgram programLight;
	QGLShaderProgram programDark;

    std::unique_ptr<SphereEngine> theSun;
    std::unique_ptr<SphereEngine> aSun;
    std::unique_ptr<SphereEngine> theSky;
    std::vector<std::unique_ptr<PlanetEngine>> planets;
    
	std::unordered_set<int> holdedKeys;

	QMatrix4x4 stateProjection;
	QQuaternion stateRotation;

	QVector3D cameraPosition;
	QVector2D cameraDirection;
	bool modeFps;
	qreal zNear, zFar, viewAngle;
	qreal aspect;
	bool action;

	QDateTime shiftedTime;
	QDateTime prevTime;
	float deltaTime;
};

#endif // MAINWIDGET_H
