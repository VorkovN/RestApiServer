#include "JsonProtocolConverter.h"

#include <cpprest/asyncrt_utils.h>
#include <ctime>

namespace yandex_disk {

    std::vector<File> JsonProtocolConverter::convertUpdatingRequest(const web::json::value& jsonFile) {

        std::vector<File> files{};

        if (!jsonFile.is_object()) {
            std::cout << "jsonFile isn't object" << std::endl;
            return {};
        }

        const auto &itemsValue = jsonFile.at(InputJsonFields::ITEMS);
        if (!itemsValue.is_array()) {
            std::cout << "itemsValue isn't array" << std::endl;
            return {};
        }

        for (const auto &item: itemsValue.as_array()) {

            File file{};

            if (!item.is_object()) {
                std::cout << "itemsObjectsElement isn't object" << std::endl;
                return {};
            }


            const auto &idValue = item.at(InputJsonFields::ID);
            if (!idValue.is_string()) {
                std::cout << "idValue isn't string" << std::endl;
                return {};
            }
            file.id = idValue.as_string();


            const auto &parentIdValue = item.at(InputJsonFields::PARENT_ID);
            if (!parentIdValue.is_string() && !parentIdValue.is_null()) {
                std::cout << "parentIdValue isn't string" << std::endl;
                return {};
            }

            if (!parentIdValue.is_null())
                file.parentId = parentIdValue.as_string();


            const auto &typeValue = item.at(InputJsonFields::TYPE);
            if (!typeValue.is_string()) {
                std::cout << "typeValue isn't string" << std::endl;
                return {};
            }

            if (typeValue.as_string() != File::FOLDER && typeValue.as_string() != File::FILE) {
                std::cout << "incorrect typeValue" << std::endl;
                return {};
            }

            // У типа FOLDER остальные поля отсутствуют, значит нужно пропустить их
            file.type = typeValue.as_string();
            if (file.type == File::FOLDER)
            {
                files.push_back(file);
                continue;
            }


            const auto &urlValue = item.at(InputJsonFields::URL);
            if (!urlValue.is_string()) {
                std::cout << "urlValue isn't string" << std::endl;
                return {};
            }
            file.url = urlValue.as_string();


            const auto &sizeValue = item.at(InputJsonFields::SIZE);
            if (!sizeValue.is_integer()) {
                std::cout << "sizeValue isn't int" << std::endl;
                return {};
            }
            file.size = sizeValue.as_integer();

            files.push_back(file);
        }

        const auto &updateDateValue = jsonFile.at(InputJsonFields::UPDATE_DATE);
        if (!updateDateValue.is_string()) {
            std::cout << "updateDateValue isn't string" << std::endl;
            return {};
        }

        for (auto& file: files)
            file.date  = stringToUnixTime(updateDateValue.as_string());

        return files;
    }

    web::json::value JsonProtocolConverter::convertGetRequest(const File& file) {
        web::json::value json;

        json[OutputJsonFields::ID] = web::json::value::string(file.id);
        json[OutputJsonFields::URL] = !file.url.empty()? web::json::value::string(file.url): web::json::value::null();
        json[OutputJsonFields::TYPE] = web::json::value::string(file.type);
        json[OutputJsonFields::PARENT_ID] = !file.parentId.empty()? web::json::value::string(file.parentId): web::json::value::null();
        json[OutputJsonFields::DATE] = web::json::value::string(unixTimeToString(file.date));
        json[OutputJsonFields::SIZE] = web::json::value::number(file.size);

        size_t counter = 0;
        for (const auto& childFile: file.children)
        {
            json[OutputJsonFields::CHILDREN][counter++] = convertGetRequest(childFile);
        }

        return json;
    }

    uint64_t JsonProtocolConverter::stringToUnixTime(const std::string& stringTime) {
        return (utility::datetime::from_string(stringTime, utility::datetime::ISO_8601).to_interval() - 116444736000000000) /10000;
    }

    std::string JsonProtocolConverter::unixTimeToString(uint64_t unixTime) {

        std::string ms = std::to_string(unixTime);
        ms = ms.substr(ms.size()-3);
        unixTime /= 1000;

        time_t curr_time = unixTime;
        struct tm *tmp = gmtime(&curr_time);
        return {std::to_string(tmp->tm_year+1900) + '-' + std::to_string(tmp->tm_mon) + '-' + std::to_string(tmp->tm_mday) + 'T' + std::to_string(tmp->tm_hour) + ':' + std::to_string(tmp->tm_min) + ':' + std::to_string(tmp->tm_sec) + '.' + ms + 'Z'};
    }




}