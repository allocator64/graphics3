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
	void init(QImage const &texture, QGLWidget &context);
    virtual void initGeometry() = 0;
	void draw(QGLShaderProgram &shader, QMatrix4x4 const &projection, QVector3D const &camera_pos);

	std::vector<VertexData> vertices;
	std::vector<GLushort> indices;
	GLuint vboIds[2];

	QQuaternion _rotation;
	QVector3D _position;
	GLuint _texture;
};

struct SphereEngine : public BaseEngine
{
	SphereEngine(float radius_, int count_=30);
	void initGeometry();

	float const radius;
	int const cnt;
};

struct ErehpsEngine : public SphereEngine
{
	ErehpsEngine(float radius_, int count_=30);
	void initGeometry();
};

struct PlanetEngine : public SphereEngine
{
	explicit PlanetEngine(PlanetConfig::Config const &c, QGLWidget &context);
	void changeTime(QDateTime const &t);

    std::unique_ptr<PlanetImpl> impl;
};
