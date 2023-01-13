#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class RenderingManager {

    struct Point {
        float x;
        float y;
        float z;

        Point() : x(0), y(0), z(0) {}

        Point(float xp, float yp, float zp) : x(xp), y(yp), z(zp) {}

        [[nodiscard]] bool equals(Point point) const { return ((x == point.x) && (y == point.y) && (z == point.z)); }
    };

    struct Triangle {
        Point normal;
        Point v1;
        Point v2;
        Point v3;

        Triangle(Point _normal, Point _v1, Point _v2, Point _v3) :
                normal(_normal), v1(_v1), v2(_v2), v3(_v3) {}
    };

public:
    RenderingManager();

    virtual ~RenderingManager();

    void InitializeVAO();

    void SetVertices(std::vector<glm::vec3>);

    void SetNormals(std::vector<glm::vec3>);

    void SetTexture(std::vector<glm::vec2>, GLubyte textureData[]);

    void SetTexture(std::vector<glm::vec2>, const std::string &bmpPath);

    void DrawObject() const;


    GLuint VertexArrayID{};
    int VertexBufferSize{};

    glm::mat4 model;

    GLuint vertexBuffer{};
    GLuint normalBuffer{};

    GLuint uvBuffer{};
    GLuint texID{};
    GLuint textureSamplerID{};

    static void computeTrianglesVertices(std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals);

    static std::vector<glm::vec3> getTriangleNormals(Point vertex, const std::vector<Triangle> &triangles);

    static glm::vec3 calculateMeanVector(const std::vector<glm::vec3> &);
};

#endif