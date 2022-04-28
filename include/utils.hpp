#pragma once

#include "common.hpp"

namespace Synthium {
    namespace FileUtils {
        inline string read_file_into_string(string filename) {
            std::ifstream stream(filename);
            string content((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));

            return content;
        }
    }
}
