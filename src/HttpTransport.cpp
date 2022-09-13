#include "HttpTransport.h"

#include "File.h"

namespace yandex_disk {

//0.0.0.0:80
    HttpTransport::HttpTransport() :
            _httpListenerImports(BASE_URL + PREFIX_IMPORTS),
            _httpListenerDelete(BASE_URL + PREFIX_DELETE),
            _httpListenerNodes(BASE_URL + PREFIX_NODES) {

        _httpListenerImports.support(methods::POST,[this](auto &&message) { postHandler(std::move(message)); });
        _httpListenerDelete.support(methods::DEL,[this](auto &&message) { deleteHandler(std::move(message)); });
        _httpListenerNodes.support(methods::GET, [this](auto &&message) { getHandler(std::move(message)); });

    }

    void HttpTransport::start() {
        _httpListenerImports.open().wait();
        _httpListenerDelete.open().wait();
        _httpListenerNodes.open().wait();

        std::cin.ignore();
    }


    void HttpTransport::getHandler(http_request&& message) {

        std::cout << message.to_string() << std::endl;

//        auto j = web::json::value::parse("true");
//        message.reply(200, j);
    }

    void HttpTransport::postHandler(http_request&& message) {

        File outputFile{};
        const auto& jsonFile = message.extract_json().get();

        std::cout << message.to_string() << std::endl;
        std::cout << jsonFile << std::endl;


        if (!jsonFile.is_object()){
            std::cout << "jsonFile isn't object" << std::endl;
            return;
        }

        const auto& itemsValue = jsonFile.at("items");
        if (!itemsValue.is_array()){
            std::cout << "itemsValue isn't array" << std::endl;
            return;
        }

        for (const auto& item: itemsValue.as_array()) {
            if (!item.is_object()){
                std::cout << "itemsObjectsElement isn't object" << std::endl;
                return;
            }


            const auto& idValue = item.at("id");
            if (!idValue.is_string()){
                std::cout << "idValue isn't string" << std::endl;
                return;
            }
            outputFile.id = idValue.as_string();


            const auto& parentIdValue = item.at("parentId");
            if (!parentIdValue.is_string() && !parentIdValue.is_null()){
                std::cout << "parentIdValue isn't string" << std::endl;
                return;
            }

            if (!parentIdValue.is_null())
                outputFile.parentId = parentIdValue.as_string();


            const auto& typeValue = item.at("type");
            if (!typeValue.is_string()){
                std::cout << "typeValue isn't string" << std::endl;
                return;
            }

            if (typeValue.as_string() != "FOLDER" && typeValue.as_string() != "FILE"){
                std::cout << "incorrect typeValue" << std::endl;
                return;
            }

            // У типа FOLDER остальные поля отсутствуют, значит нужно пропустить их
            if (typeValue.as_string() == "FOLDER"){
                outputFile.fileType = FileType::FOLDER;
                continue;
            } else {
                outputFile.fileType = FileType::FILE;
            }


            const auto& urlValue = item.at("url");
            if (!urlValue.is_string()){
                std::cout << "urlValue isn't string" << std::endl;
                return;
            }
            outputFile.url = urlValue.as_string();


            const auto& sizeValue = item.at("size");
            if (!sizeValue.is_integer()){
                std::cout << "sizeValue isn't int" << std::endl;
                return;
            }
            outputFile.size = sizeValue.as_integer();
        }

        const auto& updateDateValue = jsonFile.at("updateDate");
        if (!updateDateValue.is_string()){
            std::cout << "updateDateValue isn't string" << std::endl;
            return;
        }
        outputFile.updateDate = updateDateValue.as_string();

        message.reply(200);
    }

    void HttpTransport::deleteHandler(http_request&& message) {

        std::cout << message.to_string() << std::endl;

        message.reply(200);
    }

}

