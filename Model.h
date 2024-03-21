#include "Vertex.h"
#include "Mesh.h"
#include "ShaderProgram.h"

class Model
{
public:

    Model(const std::filesystem::path& filename);

    void Draw(ShaderProgram& shader);

private:
    std::vector<Mesh> meshes;
    std::string name;

    void LoadOBJFile(const std::filesystem::path& filename); //TODO: modify
    void LoadMTLFile(const std::filesystem::path& filename); //TODO: implement

    //GLuint LoadTexture(const std::filesystem::pathg& filename);

    //...
};

