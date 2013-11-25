#include "mainwidget.h"
#include "config.h"

#include <QMouseEvent>

#include <cmath>
#include <locale.h>

MainWidget::MainWidget(QWidget *parent) :
	QGLWidget(parent),
	action(true),
	deltaTime(1),
	shiftedTime(QDateTime::currentDateTimeUtc()),
	prevTime(QDateTime::currentDateTimeUtc()),
	modeFps(false)
{
	qDebug() << PlanetConfig::cnf[0].name;
}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
	// Save mouse press position
	if (e->button() == Qt::LeftButton) {
		modeFps = !modeFps;
		qDebug() << "modeFps: " << modeFps;
	}
	// mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *)
{
	// // Mouse release position - mouse press position
	// QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

	// // Rotation axis is perpendicular to the mouse position difference
	// // vector
	// QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

	// // Accelerate angular speed relative to the length of the mouse sweep
	// qreal acc = diff.length() / 100.0;

	// // Calculate new rotation axis as weighted sum
	// rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

	// // Increase angular speed
	// angularSpeed += acc;
}
//! [0]

void MainWidget::viewUp(float alpha)
{
	float lim = M_PI / 2.0 - M_PI / 100.0;
	cameraDirection.setY(std::min(std::max(cameraDirection.y() + alpha, -lim), lim));
}

void MainWidget::viewRight(float alpha)
{
	cameraDirection.setX(cameraDirection.x() + alpha);
	if (cameraDirection.x() < -M_PI)
		cameraDirection += QVector2D(2*M_PI, 0);
	if (cameraDirection.x() > M_PI)
		cameraDirection -= QVector2D(2*M_PI, 0);
}

QVector3D MainWidget::getDirection()
{
	QVector3D direct(
		std::cos(cameraDirection.y()) * std::sin(cameraDirection.x()),
		std::sin(cameraDirection.y()),
		-std::cos(cameraDirection.y()) * std::cos(cameraDirection.x()));
	direct.normalize();
	return direct;
}

void MainWidget::viewForward(float delta)
{
	QVector3D direct = getDirection();
	cameraPosition += direct * delta;
}

void MainWidget::modifyAngle(float alpha)
{
	viewAngle = std::min(std::max((float)viewAngle - alpha, 10.0f), 120.0f);
	stateProjection.setToIdentity();
	stateProjection.perspective(viewAngle, aspect, zNear, zFar);

}

void MainWidget::changeDeltaTime(float delta)
{
	deltaTime = std::max(1.0f, std::min(delta, 60 * 60 * 24 * 365 * 10 + .0f));
	qDebug() << "new deltaTime: " << deltaTime;
}

//! [1]
void MainWidget::timerEvent(QTimerEvent *)
{
	float alpha = .05;
	float d = cameraPosition.length();
	for (unsigned idx = 0; idx < planets.size(); ++idx)
		d = std::min(d, cameraPosition.distanceToPoint(planets[idx]->statePosition));
	float delta = std::max(d / 10, .000001f);
	QVector3D direct = getDirection();
	if (holdedKeys.count(Qt::Key_W))
		viewForward(delta);

	if (holdedKeys.count(Qt::Key_S))
		viewForward(-delta);

	if (holdedKeys.count(Qt::Key_D)) {
		QVector3D d = QVector3D::normal(direct, direct + QVector3D(0, 1, 0));
		cameraPosition += d * delta;
	}

	if (holdedKeys.count(Qt::Key_A)) {
		QVector3D d = QVector3D::normal(direct, direct + QVector3D(0, 1, 0));
		cameraPosition -= d * delta;
	}

	if (holdedKeys.count(Qt::Key_Up))
		viewUp(alpha);

	if (holdedKeys.count(Qt::Key_Down))
		viewUp(-alpha);

	if (holdedKeys.count(Qt::Key_Right))
		viewRight(alpha);

	if (holdedKeys.count(Qt::Key_Left))
		viewRight(-alpha);

	for (char num = '0'; num <= '9'; ++num)
		if (holdedKeys.count(num)) {
			QVector3D move = -cameraPosition;
			float soNear = theSun->radius;
			if (PlanetConfig::modeSurvey)
				soNear = aSun->radius;
			if (num != '0') {
				move = planets[num - '1']->statePosition - cameraPosition;
				soNear = planets[num - '1']->radius;
			}
			float x = std::atan2(move.x(), -move.z());
			float y = std::atan2(move.y(), std::pow(move.x() * move.x() + move.z() * move.z(), .5f));
			if (x == cameraDirection.x() && y == cameraDirection.y()) {
				viewForward(delta);
			} else {
				viewRight(x - cameraDirection.x());
				viewUp(y - cameraDirection.y());
			}
		}

	if (modeFps) {
		QPoint pos = QCursor::pos() - mapToGlobal(QPoint(width() / 2, height() / 2));
		viewUp(-pos.y() * .01);
		viewRight(pos.x() * .01);
		QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
	}

	if (holdedKeys.count(Qt::Key_Plus))
		modifyAngle(1);
	if (holdedKeys.count(Qt::Key_Minus))
		modifyAngle(-1);
	if (holdedKeys.count('/'))
		modifyAngle(viewAngle - 45);

	if (holdedKeys.count(Qt::Key_C))
		changeDeltaTime(deltaTime * 2);
	if (holdedKeys.count(Qt::Key_Z))
		changeDeltaTime(deltaTime / 2);
	if (holdedKeys.count(Qt::Key_X))
		changeDeltaTime(0);

	// // Decrease angular speed (friction)
	// angularSpeed *= 0.99;

	// // Stop rotation when speed goes below threshold
	// if (angularSpeed < 0.01) {
	// 	angularSpeed = 0.0;
	// } else {
	// 	// Update rotation
	// 	rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

	// 	// Update scene
	// }

	QDateTime now = QDateTime::currentDateTimeUtc();
	if (action)
		shiftedTime = shiftedTime.addMSecs(deltaTime * prevTime.msecsTo(now));
	for (unsigned idx = 0; idx < planets.size(); ++idx)
		planets[idx]->changeTime(shiftedTime);
	prevTime = now;
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
	// cube_texture = bindTexture(QImage(":/cube.png"));

	for (int idx = 0; idx < PlanetConfig::count; ++idx)
		planets.push_back(std::unique_ptr<PlanetEngine>(new PlanetEngine(this, PlanetConfig::cnf[idx])));

	theSun.reset(new SphereEngine(696342.0 / 149597870.691));
	theSun->init(this, QImage(":/sun"));

	aSun.reset(new SphereEngine(0.0002));
	aSun->init(this, QImage(":/sun"));

	theSky.reset(new SphereEngine(10, /* inverted */ true));
	theSky->init(this, QImage(":/sky"));
}

void MainWidget::keyPressEvent(QKeyEvent *key)
{
	if (key->key() == Qt::Key_Space) {
		action = !action;
		qDebug() << "Action: " << action;
	}
	if (key->key() == Qt::Key_V) {
		PlanetConfig::modeSurvey = !PlanetConfig::modeSurvey;
		deltaTime = 1;
		qDebug() << "modeSurvey: " << PlanetConfig::modeSurvey;
	}
	holdedKeys.insert(key->key());
}

void MainWidget::keyReleaseEvent(QKeyEvent *key)
{
	holdedKeys.erase(key->key());
}

void MainWidget::resizeGL(int w, int h)
{
	// Set OpenGL viewport to cover whole widget
	glViewport(0, 0, w, h);

	// Calculate aspect ratio
	aspect = qreal(w) / qreal(h ? h : 1);

	// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
	zNear = 0.00002, zFar = 15.0, viewAngle = 45.0;

	// Reset projection
	stateProjection.setToIdentity();

	// Set perspective projection
	stateProjection.perspective(viewAngle, aspect, zNear, zFar);
}
//! [5]

void MainWidget::paintGL()
{
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate model view transformation
	QMatrix4x4 cameraRotation;
	cameraRotation.rotate(-cameraDirection.y() / M_PI * 180, 1, 0, 0);
	cameraRotation.rotate(cameraDirection.x() / M_PI * 180, 0, 1, 0);

	QMatrix4x4 currentProjection = stateProjection * cameraRotation;

	programDark.bind();
	if (!PlanetConfig::modeSurvey) {
		theSun->draw(programDark, currentProjection, cameraPosition);
		theSky->statePosition = cameraPosition;
		theSky->draw(programDark, currentProjection, cameraPosition);
	} else 
		aSun->draw(programDark, currentProjection, cameraPosition);

	programLight.bind();
	programLight.setUniformValue("eyePos", QVector4D(cameraPosition, 0));
	programLight.setUniformValue("lightPos", QVector4D(-cameraPosition, 1));

	// Draw cube geometry
	for (unsigned idx = 0; idx < planets.size(); ++idx)
		planets[idx]->draw(programLight, currentProjection, cameraPosition);
}
