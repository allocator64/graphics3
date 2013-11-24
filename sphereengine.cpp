#include "sphereengine.h"

#include <QVector2D>
#include <QVector3D>

#include <vector>
#include <cmath>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
    QVector3D normal;
};

SphereEngine::SphereEngine()
{
}

SphereEngine::~SphereEngine()
{
    glDeleteBuffers(2, vboIds);
}

void SphereEngine::init()
{
    initializeGLFunctions();

//! [0]
    // Generate 2 VBOs
    glGenBuffers(2, vboIds);

//! [0]

    // Initializes cube geometry and transfers it to VBOs
    initSphereGeometry();
}

void SphereEngine::initSphereGeometry()
{
//    std::vector<VertexData> vertices;
//    std::vector<GLushort> indices;

//    const int count = 10;
//    const float r = 1;
//    const float d_teta = M_PI / count;
//    const float d_ksi = 2*M_PI / count;

//    for (float teta = 0; teta <= M_PI; teta += d_teta)
//        for (float ksi = 0; ksi <= 2*M_PI; ksi += d_ksi) {
//            VertexData data;
//            data.position = QVector3D(
//                        r*sin(teta)*cos(ksi),
//                        r*sin(teta)*sin(ksi),
//                        r*cos(teta));
//            data.texCoord = QVector2D(ksi / 2 / M_PI, teta / M_PI);
//            data.normal = data.position;
//            vertices.push_back(data);
//            qDebug() << data.position;
//        }
    const int cnt = 30;
    const float radius = 1;
    std::vector<VertexData> vertices((2*cnt + 1)*(2*cnt + 1));
    std::vector<GLushort> indices;

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

//! [1]
    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    indices_size = indices.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size*sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
//! [1]
}

//! [2]
void SphereEngine::drawSphereGeometry(QGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    offset += sizeof(QVector2D);
    int normalLocation = program->attributeLocation("a_normcoord");
    program->enableAttributeArray(normalLocation);
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Draw cube geometry using indices from VBO 1
//    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLE_STRIP, indices_size, GL_UNSIGNED_SHORT, 0);
}
//! [2]
