#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include "File.h"

namespace yandex_disk {


    using namespace web::http;

    class HttpTransport {

        using HttpListener = experimental::listener::http_listener;
    public:
        HttpTransport();

        void start();

    private:
        void getHandler(http_request&& message);

        void postHandler(http_request&& message);

        void deleteHandler(http_request&& message);

    private:
        const std::string BASE_URL = "http://localhost:8080/";
        const std::string PREFIX_IMPORTS = "imports";
        const std::string PREFIX_DELETE = "delete";
        const std::string PREFIX_NODES = "nodes";

        HttpListener _httpListenerImports;
        HttpListener _httpListenerDelete;
        HttpListener _httpListenerNodes;
    };

}