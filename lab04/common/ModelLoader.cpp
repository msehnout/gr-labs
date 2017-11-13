#include <iostream>
#include <sstream>
//#include <unordered_map>
#include "ModelLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "tinyxml2.h"
#include "util.h"

using namespace glm;
using namespace std;
using namespace tinyxml2;

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide :
// - Binary files. Reading a model should be just a few memcpy's away,
//   not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc
void loadOBJ(
    std::string path,
    vector<vec3>& vertices,
    vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices
) {
    cout << "Loading OBJ file: " << path << endl;

    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<vec3> temp_vertices;
    vector<vec2> temp_uvs;
    vector<vec3> temp_normals;
    indices.clear();

    FILE * file = fopen(path.c_str(), "r");
    if (file == NULL) {
        throw runtime_error("Can't open the file.\n");
    }

    while (1) {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0) {
            vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            // Invert V coordinate since we will only use DDS texture,
            // which are inverted. Remove if you want to use TGA or BMP loaders.
            uv.y = -uv.y;
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                fclose(file);
                throw runtime_error("File can't be read by our simple parser.\n");
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        vec3 vertex = temp_vertices[vertexIndex - 1];
        vec2 uv = temp_uvs[uvIndex - 1];
        vec3 normal = temp_normals[normalIndex - 1];

        // Put the attributes in buffers
        vertices.push_back(vertex);
        uvs.push_back(uv);
        normals.push_back(normal);
        indices.push_back(indices.size());
    }
    fclose(file);
}

void loadOBJWithTiny(string path,
    vector<vec3>& vertices,
    vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices)
{
    // load obj
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str());
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
        throw runtime_error(err);
    }

    //unordered_map<vec3, unsigned int> uniqueVertices = {};
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vec3 vertex = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2] };
            if (attrib.texcoords.size() != 0)
            {
                vec2 uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };
                uvs.push_back(uv);
            }
            if (attrib.normals.size() != 0)
            {
                vec3 normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2] };
                normals.push_back(normal);
            }

            // TODO must check for uniqueness
            /*if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<unsigned int>(vertices.size());
            vertices.push_back(vertex);
            }
            bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
            }
            namespace std {
            template<> struct hash<Vertex> {
            size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
            (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
            (hash<glm::vec2>()(vertex.texCoord) << 1);
            }
            };
            }
            //indices.push_back(uniqueVertices[vertex]);
            */
            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }
}

void loadVTP(string path,
    vector<vec3>& vertices, vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices)
{
    indices.clear();
    const char* method = "PolygonalMesh::loadVtpFile()";
    XMLDocument vtp;
    assert(vtp.LoadFile(path.c_str()) == 0);

    XMLElement* root = vtp.FirstChildElement("VTKFile");
    assert(root != nullptr);
    assert(root->Attribute("type", "PolyData"));

    XMLElement* polydata = root->FirstChildElement("PolyData");
    assert(polydata != nullptr);
    XMLElement* piece = polydata->FirstChildElement("Piece");
    assert(piece != nullptr);
    XMLElement* enormals = piece->FirstChildElement("PointData");
    assert(enormals != nullptr);
    XMLElement* points = piece->FirstChildElement("Points");
    assert(points != nullptr);

    int numPoints, numPolys;
    piece->QueryIntAttribute("NumberOfPoints", &numPoints);
    piece->QueryIntAttribute("NumberOfPolys", &numPolys);

    //assert(enormals->Attribute("format", "ascii"));
    const char* normalsStr = enormals->FirstChildElement("DataArray")->FirstChild()->Value();
    stringstream sNorm(normalsStr);
    vector<vec3> tempNormals;
    do {
        vec3 normal;
        sNorm >> normal.x >> normal.y >> normal.z;
        tempNormals.push_back(normal);
    } while (sNorm.good());
    assert(tempNormals.size() == numPoints + 1);

    XMLElement* pointData = points->FirstChildElement("DataArray");
    assert(pointData != nullptr);
    assert(pointData->Attribute("format", "ascii"));
    const char* coordsStr = points->FirstChildElement("DataArray")->FirstChild()->Value();

    stringstream sCoord(coordsStr);
    vector<vec3> coordinates;
    do {
        vec3 coord;
        sCoord >> coord.x >> coord.y >> coord.z;
        coordinates.push_back(coord);
    } while (sCoord.good());
    coordinates.pop_back();  // the last is corrupted
    assert(coordinates.size() == numPoints);

    XMLElement* polys = piece->FirstChildElement("Polys");
    assert(polys != nullptr);

    // TODO beter implementation
    XMLElement *econnectivity, *eoffsets;
    if (polys->FirstChildElement("DataArray")->Attribute("Name", "connectivity"))
    {
        econnectivity = polys->FirstChildElement("DataArray");
        assert(econnectivity != nullptr);
        assert(econnectivity->Attribute("format", "ascii"));
    }
    else
    {
        throw runtime_error("Can't access connectivity");
    }

    if (polys->LastChildElement("DataArray")->Attribute("Name", "offsets"))
    {
        eoffsets = polys->LastChildElement("DataArray");
        assert(eoffsets != nullptr);
        assert(eoffsets->Attribute("format", "ascii"));
    }
    else
    {
        throw runtime_error("Can't access offsets");
    }

    // read offsets
    const char* offsetsStr = eoffsets->FirstChild()->Value();
    stringstream sOffsets(offsetsStr);
    vector<int> offsets;
    do {
        int offset;
        sOffsets >> offset;
        offsets.push_back(offset);
    } while (sOffsets.good());
    assert(offsets.size() == numPolys + 1);

    // read connectivity
    const char* connStr = econnectivity->FirstChild()->Value();
    stringstream sConn(connStr);
    vector<int> connectivity;
    do {
        int conn;
        sConn >> conn;
        connectivity.push_back(conn);
    } while (sConn.good());
    assert(connectivity.size() == offsets.back() + 1);

    // construct vertices
    int startPoly = 0;
    for (int i = 0; i < numPolys; ++i) {
        vector<int> face = slice(connectivity, startPoly, offsets[i]);
        int i1 = 0, i2 = 1, i3 = 2;
        while (i3 != face.size())
        {
            vertices.push_back(coordinates[face[i1]]);
            normals.push_back(tempNormals[face[i1]]);
            indices.push_back(indices.size());
            vertices.push_back(coordinates[face[i2]]);
            normals.push_back(tempNormals[face[i2]]);
            indices.push_back(indices.size());
            vertices.push_back(coordinates[face[i3]]);
            normals.push_back(tempNormals[face[i3]]);
            indices.push_back(indices.size());
            i2++;
            i3++;
        }
        startPoly = offsets[i];
    }
}

Drawable::Drawable(string path)
{
    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;
    vector<unsigned int> indices;

    if (path.substr(path.size() - 3, 3) == "obj")
    {
        loadOBJWithTiny(path.c_str(), vertices, uvs, normals, indices);
    }
    else if (path.substr(path.size() - 3, 3) == "vtp")
    {
        loadVTP(path.c_str(), vertices, uvs, normals, indices);
    }
    else
    {
        throw runtime_error("File format not supported: " + path);
    }
    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex VBO
    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3),
        &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // uvs VBO
    if (uvs.size() != 0)
    {
        glGenBuffers(1, &uvsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2),
            &uvs[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    // normals VBO
    if (normals.size() != 0)
    {
        glGenBuffers(1, &normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3),
            &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    // Generate a buffer for the indices
    numIndices = indices.size();
    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);
}

Drawable::~Drawable()
{
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &uvsVBO);
    glDeleteBuffers(1, &normalsVBO);
    glDeleteBuffers(1, &elementVBO);
    glDeleteBuffers(1, &VAO);
}

void Drawable::bind()
{
    glBindVertexArray(VAO);
}

void Drawable::draw()
{
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);
    //glDrawArrays(GL_TRIANGLES, 0, numIndices);
}
