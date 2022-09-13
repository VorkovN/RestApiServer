#pragma once

#include <optional>
#include <cpprest/json.h>

#include "File.h"

namespace yandex_disk {

    class JsonProtocolConverter {

    public:
        static std::optional<File> convertUpdatingRequest(const web::json::value& jsonFile);
        static web::json::value convertGetRequest(const File& file);
    };

}