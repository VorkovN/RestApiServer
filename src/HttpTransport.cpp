#include "HttpTransport.h"

#include "File.h"
#include "JsonProtocolConverter.h"

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

    void HttpTransport::initialize(IDiskFacade* diskFacade)
    {
        _diskFacade = diskFacade;
    }

    void HttpTransport::start() {
        _httpListenerImports.open().wait();
        _httpListenerDelete.open().wait();
        _httpListenerNodes.open().wait();

        std::cin.ignore();
    }


    void HttpTransport::getHandler(http_request&& message) {

        std::string idString = message.relative_uri().to_string().substr(1);

        if (idString.find('/') != std::string::npos){
            message.reply(400, "Validation Failed");
            return;
        }

        auto fileOpt = _diskFacade->getNode(idString);
        if (!fileOpt.has_value())
            message.reply(404, "Item not found");

        auto jsonFile = JsonProtocolConverter::convertGetRequest(fileOpt.value());


        message.reply(200, jsonFile);
    }

    void HttpTransport::postHandler(http_request&& message) {

        const web::json::value& jsonFile = message.extract_json().get();

        std::cout << jsonFile << std::endl;

        auto files = JsonProtocolConverter::convertUpdatingRequest(jsonFile);

        if (files.empty()){
            message.reply(400, "Validation Failed");
            return;
        }

        bool successfulResult = true;
        for (const auto& file: files)
            successfulResult &= _diskFacade->postNode(file);

        if (!successfulResult){
            message.reply(400, "Validation Failed");
            return;
        }

        message.reply(200);
    }

    void HttpTransport::deleteHandler(http_request&& message) {

        std::string idString = message.relative_uri().to_string().substr(1);

        if (idString.find('/') != std::string::npos){
            message.reply(400, "Validation Failed");
            return;
        }

        if (!_diskFacade->deleteNode(idString))
            message.reply(404, "Item not found");

        message.reply(200);
    }

}

