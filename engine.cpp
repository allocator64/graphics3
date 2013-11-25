#include "engine.h"

void BaseEngine::init(QImage const &texture, QGLWidget &context)
{
    // qDebug() << texture.size();

	initializeGLFunctions();

//! [0]
	// Generate 2 VBOs
	glGenBuffers(2, vboIds);

//! [0]

	// Initializes cube geometry and transfers it to VBOs
	initGeometry();

//! [1]
	// Transfer vertex data to VBO 0
	glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);

	// Transfer index data to VBO 1
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
//! [1]

	// init texture
	glEnable(GL_TEXTURE_2D);

	_texture = context.bindTexture(texture);

	// Set nearest filtering mode for texture minification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set bilinear filtering mode for texture magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void BaseEngine::draw(QGLShaderProgram &shader, QMatrix4x4 const &projection, QVector3D const &shift_camera)
{
	// Tell OpenGL which VBOs to use
	glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
	

	QMatrix4x4 matrix;
	matrix.translate(_position - shift_camera);
	matrix.rotate(_rotation);

	QMatrix4x4 matrix_normal;
	matrix_normal.rotate(_rotation);

	shader.setUniformValue("projection_model_view_matrix", projection * matrix);
	shader.setUniformValue("model_view_matrix", matrix);
	shader.setUniformValue("normal_matrix", matrix_normal);

	// Offset for position
	quintptr offset = 0;

	// Tell OpenGL programmable pipeline how to locate vertex position data
	int vertexLocation = shader.attributeLocation("a_position");
	shader.enableAttributeArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void const *)offset);

	// Offset for texture coordinate
	offset += sizeof(QVector3D);

	// Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = shader.attributeLocation("a_texcoord");
	shader.enableAttributeArray(texcoordLocation);
	glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void const *)offset);

	offset += sizeof(QVector2D);
	int normalLocation = shader.attributeLocation("a_normcoord");
	shader.enableAttributeArray(normalLocation);
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void const *)offset);

	// Set texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	shader.setUniformValue("texture", 0);

	// Draw cube geometry using indices from VBO 1
	// glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_SHORT, 0);
}

SphereEngine::SphereEngine(float radius_, int count_)
	: radius(radius_),
	  cnt(count_)
{
}

void SphereEngine::initGeometry()
{
	vertices.resize((2*cnt + 1)*(2*cnt + 1));
	indices.clear();

	for (int i = -cnt; i <= cnt; ++i) {
		float ksi = (.0 + i) / cnt*M_PI / 2.0;
		float y = std::sin(ksi)*radius;
		for (int j = 0; j <= 2*cnt; ++j) {
			float phi = (.0 + j) / cnt*M_PI;
			float x = std::cos(ksi)*sin(phi)*radius;
			float z = -std::cos(ksi)*cos(phi)*radius;
			int idx = (2*cnt + 1)*(i + cnt) + j;
			int idx2 = (2*cnt + 1)*(i + cnt - 1) + j;
			vertices[idx].position = QVector3D(x, y, z);
			vertices[idx].normal = QVector3D(x, y, z);
			vertices[idx].texCoord = QVector2D(phi/(2*M_PI), (ksi+(M_PI/2.0))/M_PI);
			if (i != -cnt && j != 2*cnt) {
				indices.push_back(idx);
				indices.push_back(idx + 1);
				indices.push_back(idx2);
				indices.push_back(idx2);
				indices.push_back(idx2 + 1);
				indices.push_back(idx + 1);
			}
		}
	}
}

PlanetEngine::PlanetEngine(PlanetConfig::Config const &c, QGLWidget &context)
	: SphereEngine(c.initial_inner_rad / 149597870.691)
{
	impl.reset(new PlanetImpl(c));
	init(QImage(":/" + c.name), context);
	changeTime(QDateTime::currentDateTimeUtc());
}

void PlanetEngine::changeTime(QDateTime const &t)
{
    _position = impl->getPosition(t);
    _rotation = QQuaternion::fromAxisAndAngle(0, 1, 0, impl->getRotationAngle(t));
}
