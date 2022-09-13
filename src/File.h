#pragma once

#include <list>
#include <string>

namespace yandex_disk {

    enum FileType : bool {
        FILE, FOLDER
    };

    struct File {
        FileType fileType;
        std::string url;
        std::string id;
        std::string parentId;
        int64_t size;
        std::string updateDate;
        std::list<File> childFiles;
    };

}