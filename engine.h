#pragma once;

#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>

#include <vector>
#include <cmath>

#include "ephemeris.h"

struct BaseEngine : public QGLFunctions
{
	struct VertexData
	{
		QVector3D position;
		QVector2D texCoord;
		QVector3D normal;
	};
	void init(QGLWidget *that, QImage const &texture);
	void draw(QGLShaderProgram &program, QMatrix4x4 const &stateProjection, QVector3D const &stateCameraPosition);
    virtual void initGeometry() = 0;

	std::vector<VertexData> vertices;
	std::vector<GLushort> indices;
	GLuint vboIds[2];
    GLuint textureIdx;

    QQuaternion stateRotation;
    QVector3D statePosition;
};

struct SphereEngine : public BaseEngine
{
	explicit SphereEngine(float radius_, bool inverted_=false, int count_=30);
	void initGeometry();

	float const radius;
	int const cnt;
	bool const inverted;
};

struct PlanetEngine : public SphereEngine
{
	PlanetEngine(QGLWidget *that_, PlanetConfig::Config const &cnf_);
	void changeTime(QDateTime const &newTime);

    std::unique_ptr<PlanetImpl> impl;
};
