#include "engine.h"

void BaseEngine::init(QGLWidget *that, QImage const &texture)
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

	textureIdx = that->bindTexture(texture);

	// Set nearest filtering mode for texture minification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set bilinear filtering mode for texture magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void BaseEngine::draw(QGLShaderProgram &program, QMatrix4x4 const &stateProjection, QVector3D const &stateCameraPosition)
{
	// Tell OpenGL which VBOs to use
	glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
	

	QMatrix4x4 matrix;
    matrix.translate(statePosition - stateCameraPosition);
    matrix.rotate(stateRotation);

	QMatrix4x4 normalMatrix;
    normalMatrix.rotate(stateRotation);

	program.setUniformValue("projection_model_view_matrix", stateProjection * matrix);
	program.setUniformValue("model_view_matrix", matrix);
	program.setUniformValue("normal_matrix", normalMatrix);

	// Offset for position
	quintptr offset = 0;

	// Tell OpenGL programmable pipeline how to locate vertex position data
	int vertexLocation = program.attributeLocation("a_position");
	program.enableAttributeArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void const *)offset);

	// Offset for texture coordinate
	offset += sizeof(QVector3D);

	// Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = program.attributeLocation("a_texcoord");
	program.enableAttributeArray(texcoordLocation);
	glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void const *)offset);

	offset += sizeof(QVector2D);
	int normalLocation = program.attributeLocation("a_normcoord");
	program.enableAttributeArray(normalLocation);
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void const *)offset);

	// Set texture
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIdx);
	program.setUniformValue("texture", 0);

	// Draw cube geometry using indices from VBO 1
	// glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
}

SphereEngine::SphereEngine(float radius_, bool inverted_, int count_)
	:radius(radius_),
	cnt(count_),
	inverted(inverted_)
{}

void SphereEngine::initGeometry()
{
	vertices.resize((2*cnt + 1)*(2*cnt + 1));
	indices.clear();

	for (int i = -cnt; i <= cnt; ++i) {
		float ksi = (.0 + i)*M_PI/cnt/2.0;
		float y = std::sin(ksi)*radius;
		for (int j = 0; j <= 2*cnt; ++j) {
			float phi = (.0 + j)*M_PI/cnt;
			float x = std::cos(ksi)*std::sin(phi)*radius;
			float z = std::cos(ksi)*std::cos(phi)*radius;
			if (inverted)
				z *= -1;
			int idx = (2*cnt + 1)*(i + cnt) + j;
			int idx2 = (2*cnt + 1)*(i + cnt - 1) + j;
			vertices[idx].position = QVector3D(x, y, z);
			vertices[idx].normal = QVector3D(x, y, z);
			vertices[idx].texCoord = QVector2D(phi/(2*M_PI), (ksi+(M_PI/2.0))/M_PI);
			if (i != -cnt && j != 2*cnt) {
				indices.push_back(idx);
				indices.push_back(idx2);
				indices.push_back(idx + 1);
				indices.push_back(idx2);
				indices.push_back(idx2 + 1);
				indices.push_back(idx + 1);
			}
		}
	}
}

PlanetEngine::PlanetEngine(QGLWidget *that_, PlanetConfig::Config const &cnf_)
	: SphereEngine(cnf_.initial_inner_rad / 149597870.691)
{
	impl.reset(new PlanetImpl(cnf_));
	init(that_, QImage(":/" + cnf_.name));
	changeTime(QDateTime::currentDateTimeUtc());
}

void PlanetEngine::changeTime(QDateTime const &newTime)
{
    statePosition = impl->getPosition(newTime);
	stateRotation = QQuaternion::fromAxisAndAngle(0, 1, 0, impl->getRotationAngle(newTime));
}
