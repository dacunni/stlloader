#define STLLOADER_IMPLEMENTATION
#include "stlloader.h"

int main(int argc, char ** argv)
{
    if(argc < 2) {
        std::cerr << "Please supply an input file\n";
        return EXIT_FAILURE;
    }
    if(argc < 3) {
        std::cerr << "Please supply an output file\n";
        return EXIT_FAILURE;
    }

    stlloader::Format format = stlloader::Format::ascii;
    if(argc > 3) {
        std::string format_arg(argv[3]);
        if(format_arg == "binary") format = stlloader::Format::binary;
        else if(format_arg == "ascii") format = stlloader::Format::ascii;
        else {
            std::cerr << "Unexpected output format: " << format_arg << '\n';
            return EXIT_FAILURE;
        }
    }

    stlloader::Mesh mesh;
    stlloader::parse_file(argv[1], mesh);
    stlloader::write_file(argv[2], mesh, format);

    return EXIT_SUCCESS;
}

