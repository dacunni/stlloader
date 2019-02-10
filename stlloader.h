// Single file header-only library for loading STL 3D models in either
// ASCII or binary formats.
//
// MIT License
// 
// Copyright (c) 2019 David Cunningham
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

namespace stlloader {

struct Vertex { float x, y, z; };
struct Normal { float x, y, z; };

struct Facet {
    Vertex vertices[3];
    Normal normal;
};

struct Mesh {
    std::vector<Facet> facets;
    std::string name;
    std::string header;
};

void parse_stream(std::istream & is, Mesh & mesh);
void parse_file(const char * filename, Mesh & mesh);

#ifdef STLLOADER_IMPLEMENTATION

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
    std::cout << "name: " << mesh.name << '\n';
    std::cout << "header: " << mesh.header << '\n';
    std::cout << "num facets: " << mesh.facets.size() << '\n';
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

void parse_ascii_file(std::istream & is, Mesh & mesh)
{
    std::string line, kind;
    while(std::getline(is, line)) {
        std::istringstream ss(line);
        ss >> kind;
        if(kind == "solid") {
            ss >> mesh.name;
            parse_ascii_solid(is, mesh);
        }
        else { throw; }
    }
}

template<typename T> T read_binary_value(std::istream & is);

template<> uint16_t read_binary_value(std::istream & is) {
    uint16_t value; is.read((char*)&value, 2); return value;
}

template<> uint32_t read_binary_value(std::istream & is) {
    uint32_t value; is.read((char*)&value, 4); return value;
}

template<> float read_binary_value(std::istream & is) {
    float value; is.read((char*)&value, 4); return value;
}

template<> Normal read_binary_value(std::istream & is) {
    Normal n;
    n.x = read_binary_value<float>(is);
    n.y = read_binary_value<float>(is);
    n.z = read_binary_value<float>(is);
    return n;
}

template<> Vertex read_binary_value(std::istream & is) {
    Vertex v;
    v.x = read_binary_value<float>(is);
    v.y = read_binary_value<float>(is);
    v.z = read_binary_value<float>(is);
    return v;
}

const size_t STL_BINARY_HDR_SIZE = 80;

void parse_binary_file(std::istream & is, Mesh & mesh)
{
    char header[STL_BINARY_HDR_SIZE + 1]; // header plus null byte
    is.read(header, STL_BINARY_HDR_SIZE);
    header[STL_BINARY_HDR_SIZE] = '\0'; // null terminate just in case
    mesh.header = header;

    auto num_triangles = read_binary_value<uint32_t>(is);

    for(uint32_t ti = 0; ti < num_triangles; ++ti) {
        Facet facet = {};
        facet.normal = read_binary_value<Normal>(is);
        for(int vi = 0; vi < 3; ++vi) {
            facet.vertices[vi] = read_binary_value<Vertex>(is);
        }
        // This field is unused, but must be present
        auto attrib_byte_count = read_binary_value<uint16_t>(is);
        mesh.facets.push_back(facet);
    }
}

void parse_stream(std::istream & is, Mesh & mesh)
{
    // Read enough of file to determine its type.
    char header_start[6] = "";
    is.read(header_start, 5); header_start[5] = '\0';
    // Rewind so parsers can start at the beginning.
    is.seekg(0);

    // Ascii files start with "solid". Binary files have an
    // arbitrary 80 character header.
    const bool is_ascii = (std::string(header_start) == "solid");

    if(is_ascii) {
        parse_ascii_file(is, mesh);
    }
    else {
        parse_binary_file(is, mesh);
    }
}

void parse_file(const char * filename, Mesh & mesh)
{
    std::ifstream ifs(filename, std::ifstream::binary);
    parse_stream(ifs, mesh);
}
#endif // STLLOADER_IMPLEMENTATION

} // namespace stlloader

