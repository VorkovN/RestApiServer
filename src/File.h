#pragma once

#include <list>
#include <string>

namespace yandex_disk {

    struct File {
        std::string id;
        std::string url;
        std::string type;
        std::string parentId;
        uint64_t date;
        int64_t size;
        std::list<File> children;

        inline static const std::string FILE = "FILE";
        inline static const std::string FOLDER = "FOLDER" ;
    };

}