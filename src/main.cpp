#include "../include/common.hpp"

#include "../include/pair.hpp"

i32 main(i32 argc, char **argv) {
    Synthium::Pair<string, string> pair ("John", "Doe");

    std::cout << pair.get_key() << " " << pair.get_value();

    return 0;
}