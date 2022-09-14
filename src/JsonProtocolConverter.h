#pragma once

#include <vector>
#include <cpprest/json.h>

#include "File.h"

namespace yandex_disk {

    class JsonProtocolConverter {

    public:
        static std::vector<File> convertUpdatingRequest(const web::json::value& jsonFile);
        static web::json::value convertGetRequest(const File& file);

    private:
        struct InputJsonFields
        {
            inline static const std::string ID = "id";
            inline static const std::string URL = "url";
            inline static const std::string TYPE = "type";
            inline static const std::string PARENT_ID = "parentId";
            inline static const std::string DATE = "date";
            inline static const std::string SIZE = "size";
            inline static const std::string ITEMS = "items";
            inline static const std::string UPDATE_DATE = "updateDate";
        };

        struct OutputJsonFields
        {
            inline static const std::string ID = "id";
            inline static const std::string URL = "url";
            inline static const std::string TYPE = "type";
            inline static const std::string PARENT_ID = "parentId";
            inline static const std::string DATE = "date";
            inline static const std::string SIZE = "size";
            inline static const std::string CHILDREN = "children";
        };


    };

}