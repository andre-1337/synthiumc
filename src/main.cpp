// project includes
#include "../include/pair.hpp"
#include "../include/utils.hpp"
#include "../include/common.hpp"

// third party includes
#include "../third_party/RSJparser.hpp"

i32 main([[maybe_unused]] i32 argc, [[maybe_unused]] char *argv[]) {
    Synthium::Pair<string, string> pair("John", "Doe");

    std::cout << pair.get_key() << " " << pair.get_value() << "\n\n";

    string file = Synthium::FileUtils::read_file_into_string("./tests/config.json");
    RSJresource parsed_file(file);

    std::cout << parsed_file["compiler_info"]["version"].as<string>() << "\n\n";

    return 0;
}
