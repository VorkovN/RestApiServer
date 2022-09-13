#pragma once

#include <pqxx/pqxx>

#include "File.h"

namespace yandex_disk {

    class DbController {
    public:
        DbController();
        ~DbController();

        bool initialize();

        bool postNode(File& file);
        bool deleteNode();
        File getNode();

    private:
        std::unique_ptr<pqxx::connection> _dbConnection;
        const std::string CONNECTTION_REQUEST = "user = vorkov dbname = files_storage password = pass hostaddr = 127.0.0.1 port = 5432";
    };

}


