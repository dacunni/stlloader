#define STLLOADER_IMPLEMENTATION
#include "stlloader.h"

int main(int argc, char ** argv)
{
    if(argc < 2) {
        std::cerr << "Please supply an argument\n";
        return EXIT_FAILURE;
    }
    stlloader::Mesh mesh;
    stlloader::parse_file(argv[1], mesh);
    stlloader::print(mesh);

    return EXIT_SUCCESS;
}

