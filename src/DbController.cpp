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

    bool DbController::postNode(File &file) {
        return true;
    }

    bool DbController::deleteNode() {
        return true;
    }

    File DbController::getNode() {
        return File();
    }
}
