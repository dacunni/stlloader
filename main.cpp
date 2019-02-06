#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

struct Vertex { float x, y, z; };
struct Normal { float x, y, z; };
struct Facet {
    Vertex vertices[3];
    Normal normal;
};

struct Mesh {
    std::vector<Facet> facets;
};

std::istream & operator>>(std::istream & is, Vertex & v) { return is >> v.x >> v.y >> v.z; }
std::istream & operator>>(std::istream & is, Normal & n) { return is >> n.x >> n.y >> n.z; }

void print(const Facet & facet)
{
    auto & n = facet.normal;
    printf("n %f %f %f\n", n.x, n.y, n.z);
    for(int i = 0; i < 3; ++i) {
        auto & v = facet.vertices[i];
        printf(" v%d %f %f %f\n", i, v.x, v.y, v.z);
    }
}

void print(const Mesh & mesh)
{
    for(auto & facet : mesh.facets) {
        print(facet);
    }
}

void consume(std::istream & is, const std::string & expected)
{
    std::string line, label;
    std::getline(is, line);
    std::istringstream ss(line);
    ss >> label;
    if(label != expected)
        throw;
}

std::istringstream getlinestream(std::istream & is)
{
    std::string line;
    std::getline(is, line);
    return std::istringstream(line);
}

void parse_ascii_facet(std::istream & is, Facet & facet)
{
    std::string label, sub;
    auto ss = getlinestream(is);
    ss >> label >> sub;
    if(label == "outer" && sub == "loop") {
        int vi = 0;
        do {
            auto ss = getlinestream(is);
            ss >> label;
            if(label == "vertex") {
                if(vi < 3) {
                    ss >> facet.vertices[vi];
                }
                ++vi;
            }
        } while(label != "endloop");
        if(vi > 3)
            throw; // only support triangles
    }
    else { throw; }
    consume(is, "endfacet");
}

void parse_ascii_solid(std::istream & is, Mesh & mesh)
{
    std::string line, kind, param;

    while(std::getline(is, line)) {
        std::istringstream ss(line);
        ss >> kind;
        if(kind == "endsolid") {
            break;
        }
        else if(kind == "facet") {
            Facet facet = {};
            ss >> param;
            if(param != "normal")
                throw;
            ss >> facet.normal;
            parse_ascii_facet(is, facet);
            mesh.facets.push_back(facet);
        }
        else { throw; }
    }
}

void parse_file(std::istream & is, Mesh & mesh)
{
    std::string line, kind, name;

    while(std::getline(is, line)) {
        std::istringstream ss(line);
        ss >> kind;
        if(kind == "solid") {
            ss >> name;
            parse_ascii_solid(is, mesh);
        }
        else { throw; }
    }
}

int main(int argc, char ** argv)
{
    if(argc < 2) {
        std::cerr << "Please supply an argument\n";
        return EXIT_FAILURE;
    }
    std::ifstream ifs(argv[1]);
    Mesh mesh;
    parse_file(ifs, mesh);
    print(mesh);

    return EXIT_SUCCESS;
}

