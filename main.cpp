#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

struct Facet
{
    float vertices[3][3];
    float normal[3];
};

void print(const Facet & facet)
{
    auto & n = facet.normal;
    printf("n %f %f %f\n", n[0], n[1], n[2]);
    for(int i = 0; i < 3; ++i) {
        auto & v = facet.vertices[i];
        printf(" v%d %f %f %f\n", i, v[0], v[1], v[2]);
    }
}

void print(const std::vector<Facet> & facets)
{
    for(auto & facet : facets) {
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

void parse_facet(std::istream & is, Facet & facet)
{
    std::string line, label, sub;
    std::getline(is, line);
    std::istringstream ss(line);
    ss >> label >> sub;
    if(label != "outer" && sub != "loop")
        throw;
    for(int i = 0; i < 3; ++i) {
        std::getline(is, line);
        ss.str(line);
        ss.clear();
        ss >> label;
        if(label != "vertex")
            throw;
        for(int j = 0; j < 3; ++j) {
            ss >> facet.vertices[i][j];
        }
    }
    consume(is, "endloop");
    consume(is, "endfacet");
}

void parse_solid(std::istream & is, std::vector<Facet> & facets)
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
            for(int i = 0; i < 3; ++i)
                ss >> facet.normal[i];
            parse_facet(is, facet);
            facets.push_back(facet);
        }
        else {
            throw;
        }
    }
}

void parse_file(std::istream & is, std::vector<Facet> & facets)
{
    std::string line, kind, name;

    while(std::getline(is, line)) {
        std::istringstream ss(line);
        ss >> kind;
        if(kind == "solid") {
            ss >> name;
            parse_solid(is, facets);
        }
        else {
            throw;
        }
    }
}

int main(int argc, char ** argv)
{
    if(argc < 2) {
        std::cerr << "Please supply an argument\n";
        return EXIT_FAILURE;
    }
    std::ifstream ifs(argv[1]);
    std::vector<Facet> facets;
    parse_file(ifs, facets);
    print(facets);

    return EXIT_SUCCESS;
}

