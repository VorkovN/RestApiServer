#include "JsonProtocolConverter.h"

namespace yandex_disk {

    std::optional<File> JsonProtocolConverter::convertUpdatingRequest(const web::json::value& jsonFile) {

        File file{};

        if (!jsonFile.is_object()) {
            std::cout << "jsonFile isn't object" << std::endl;
            return {};
        }

        const auto &itemsValue = jsonFile.at("items");
        if (!itemsValue.is_array()) {
            std::cout << "itemsValue isn't array" << std::endl;
            return {};
        }

        for (const auto &item: itemsValue.as_array()) {
            if (!item.is_object()) {
                std::cout << "itemsObjectsElement isn't object" << std::endl;
                return {};
            }


            const auto &idValue = item.at("id");
            if (!idValue.is_string()) {
                std::cout << "idValue isn't string" << std::endl;
                return {};
            }
            file.id = idValue.as_string();


            const auto &parentIdValue = item.at("parentId");
            if (!parentIdValue.is_string() && !parentIdValue.is_null()) {
                std::cout << "parentIdValue isn't string" << std::endl;
                return {};
            }

            if (!parentIdValue.is_null())
                file.parentId = parentIdValue.as_string();


            const auto &typeValue = item.at("type");
            if (!typeValue.is_string()) {
                std::cout << "typeValue isn't string" << std::endl;
                return {};
            }

            if (typeValue.as_string() != "FOLDER" && typeValue.as_string() != "FILE") {
                std::cout << "incorrect typeValue" << std::endl;
                return {};
            }

            // У типа FOLDER остальные поля отсутствуют, значит нужно пропустить их
            if (typeValue.as_string() == "FOLDER") {
                file.fileType = FileType::FOLDER;
                continue;
            } else {
                file.fileType = FileType::FILE;
            }


            const auto &urlValue = item.at("url");
            if (!urlValue.is_string()) {
                std::cout << "urlValue isn't string" << std::endl;
                return {};
            }
            file.url = urlValue.as_string();


            const auto &sizeValue = item.at("size");
            if (!sizeValue.is_integer()) {
                std::cout << "sizeValue isn't int" << std::endl;
                return {};
            }
            file.size = sizeValue.as_integer();
        }

        const auto &updateDateValue = jsonFile.at("updateDate");
        if (!updateDateValue.is_string()) {
            std::cout << "updateDateValue isn't string" << std::endl;
            return {};
        }
        file.updateDate = updateDateValue.as_string();

        return file;
    }

    web::json::value JsonProtocolConverter::convertGetRequest(const File& file) {
        web::json::value json;

        json["id"] = web::json::value::string(file.id);
        json["url"] = !file.url.empty()? web::json::value::string(file.url): web::json::value::null();
        json["type"] = file.fileType == FileType::FILE? web::json::value::string("FILE") : web::json::value::string("FOLDER");
        json["parentId"] = !file.parentId.empty()? web::json::value::string(file.parentId): web::json::value::null();
        json["date"] = web::json::value::string(file.updateDate);
        json["size"] = web::json::value::number(file.size);

        size_t counter = 0;
        for (const auto& childFile: file.childFiles)
        {
            json["children"][counter++] = convertGetRequest(childFile);
        }

        return json;
    }

}