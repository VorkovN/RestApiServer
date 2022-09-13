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
        }
        catch (const std::exception& e)
        {
            std::cout << "Updating DB was failed" << std::endl;
            std::cout << e.what() << std::endl;
            return false;
        }

        checkDb();
        return true;
    }

    bool DbController::deleteNode(const std::string& idString) {
        try {
            pqxx::work work(*_dbConnection);
            std::string requestString = generateDeleteRequest(idString);
            work.exec(requestString);
            work.commit();
            std::cout << "DELETE request:" << std::endl;
            std::cout << requestString << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Deleting DB element was failed" << std::endl;
            std::cout << e.what() << std::endl;
            return false;
        }

        checkDb();
        return true;
    }

    std::optional<File> DbController::getNode(const std::string& idString) {

        File file;
        try {
            pqxx::nontransaction nontransaction(*_dbConnection);
            std::string requestString = generateSelectRequest(idString);
            // Проверяем самый корневой айди на существование и достаем его из БД
            auto result  = nontransaction.exec(requestString);
            nontransaction.commit();

            auto dbElement = result.front();
            fillFields(file, dbElement);

            if (file.fileType == FileType::FOLDER)
                if (!checkChildNodes(idString, file, nontransaction))
                    return {};

            std::cout << "SELECT request:" << std::endl;
            std::cout << requestString << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Select DB target element was failed" << std::endl;
            std::cout << e.what() << std::endl;
            return {};
        }

        checkDb();
        return file;
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

    std::string DbController::generateDeleteRequest(const std::string& idString) {
        return "DELETE from files_storage where id = '" + idString + "';";
    }

    std::string DbController::generateSelectRequest(const std::string& idString) {
        return "SELECT  * from files_storage where id = '" + idString + "';"; //todo нужна ли *
    }

    std::string DbController::generateSelectChildrenRequest(const std::string& parentIdString) {
        return "SELECT * from files_storage where parent_id = '" + parentIdString + "';";
    }

    // Рекурсивный поиск в БД всех дочерних элементов
    bool DbController::checkChildNodes(const std::string& parentIdString, File& file, pqxx::nontransaction& nontransaction) {

        //запрос всех элементов с нужным родительским айди

        pqxx::result result;
        try {
            std::string requestString = generateSelectChildrenRequest(parentIdString);
            result = nontransaction.exec(requestString);
            nontransaction.commit();
        }
        catch (const std::exception& e)
        {
            std::cout << "Select DB child element was failed" << std::endl;
            std::cout << e.what() << std::endl;
            return false;
        }

        //Во время поиска дочерних элементов рекурсивно заполним сайз всех папок(хранить сайз папок в БД смысла не было)
        for (pqxx::result::const_iterator dbElement = result.begin(); dbElement != result.end(); ++dbElement) {
            File childNodeFile{};

            fillFields(childNodeFile, dbElement);

            if (file.fileType == FileType::FOLDER)
                checkChildNodes(childNodeFile.id, childNodeFile, nontransaction);

            file.size += childNodeFile.size;
            childNodeFile.childFiles.push_back(childNodeFile);
        }

        return true;
    }

    void DbController::fillFields(File &file, pqxx::result::reference& dbElement) {
        file.id = dbElement.at("id").as<std::string>();
        file.parentId = dbElement.at("parent_id").as<std::string>();
        file.url = dbElement.at("url").as<std::string>();
        file.size = dbElement.at("size").as<int>();
        file.updateDate = dbElement.at("date").as<std::string>();
        file.fileType = file.size != 0? FileType::FILE: FileType::FOLDER;
    }

    //todo delete
    void DbController::checkDb() {
        pqxx::nontransaction nontransaction(*_dbConnection);
        auto result  = nontransaction.exec("SELECT * FROM files_storage");
        for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
            std::cout << "id = " << c[0].as<std::string>() << std::endl;
        }
        nontransaction.commit();
    }



}
