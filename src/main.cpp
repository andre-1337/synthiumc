#include "../include/common.hpp"

#include "../include/pair.hpp"
#include "../include/utils.hpp"

i32 main(i32 argc, char *argv[]) {
    Synthium::Pair<string, string> pair ("John", "Doe");

    std::cout << pair.get_key() << " " << pair.get_value() << "\n\n";

    string contents = Synthium::FileUtils::read_file_into_string(argv[1]);
    std::cout << contents << "\n\n";

    return 0;
}
