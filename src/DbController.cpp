#include "DbController.h"

#include <iostream>

namespace yandex_disk {

    DbController::DbController() {
    }

    bool DbController::initialize() {
        try {
            _dbConnection = std::make_unique<pqxx::connection>(CONNECTTION_REQUEST);
        }
        catch (const std::exception& e)
        {
            std::cout << "Database initialization problem: " << std::endl;
            std::cout << e.what() << std::endl;
            return false;
        }
        if (!_dbConnection->is_open()) {
            std::cout << "Can't open database" << std::endl;
            return false;
        }

        std::cout << "Opened database successfully: " << _dbConnection->dbname() << std::endl;
        return true;
    }

    DbController::~DbController() {
        _dbConnection->disconnect();
    }

    bool DbController::postNode(const File &file) {

        try {
            pqxx::work work(*_dbConnection);
            std::string requestString = generateUpsertRequest(file);
            work.exec(requestString);
            work.commit();
            std::cout << "UPSERT request:" << std::endl;
            std::cout << requestString << std::endl;

            /////check DB
            pqxx::nontransaction nontransaction(*_dbConnection);
            auto result  = nontransaction.exec("SELECT * FROM files_storage");
            for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
                std::cout << "id = " << c[0].as<std::string>() << std::endl;
            }
            nontransaction.commit();
            /////

        }
        catch (const std::exception& e)
        {
            std::cout << "Updating DB was failed" << std::endl;
            std::cout << e.what() << std::endl;
            return false;
        }

        return true;
    }

    bool DbController::deleteNode() {

        return true;
    }

    File DbController::getNode() {
        return File();
    }

    std::string DbController::generateUpsertRequest(const File &file) {

        std::string requestString;
        if (file.fileType == FileType::FILE)
            requestString = std::string(
                    "INSERT INTO files_storage (id, parent_id, url, size, date) "
                    "VALUES ('" + file.id + "', '" + file.parentId + "', '" + file.url + "', " + std::to_string(file.size) + ", '" + file.updateDate +"') "
                    "ON CONFLICT (id) DO UPDATE SET "
                    "parent_id = excluded.parent_id, "
                    "url = excluded.url, "
                    "size = excluded.size, "
                    "date = excluded.date;");
        else
            requestString = std::string(
                    "INSERT INTO files_storage (id, parent_id, size, date) "
                    "VALUES ('" + file.id + "', '" + file.parentId + "', 0, '" + file.updateDate +"') "
                    "ON CONFLICT (id) DO UPDATE SET "
                    "parent_id = excluded.parent_id, "
                     "size = excluded.size, "
                    "date = excluded.date;");

        return requestString;
    }



}
