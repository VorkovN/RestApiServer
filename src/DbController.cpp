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

        return true;
    }

    bool DbController::deleteNode(const std::string& idString, uint64_t newDate) {
        try {

            pqxx::nontransaction nontransaction(*_dbConnection);
            std::string requestString = generateSelectRequest(idString);
            auto result  = nontransaction.exec(requestString);
            nontransaction.commit();
            if (result.empty())
                return false;
            auto updatingId = result.front().at(DbFields::PARENT_ID).as<std::string>();

            pqxx::work work(*_dbConnection);

            requestString = generateUpdateParentRequest(updatingId, newDate);
            work.exec(requestString);

            requestString = generateDeleteRequest(idString);
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

        return true;
    }

    std::optional<File> DbController::getNode(const std::string& idString) {

        File file;
        try {
            pqxx::nontransaction nontransaction(*_dbConnection);
            std::string requestString = generateSelectRequest(idString);
            // ?????????????????? ?????????? ???????????????? ???????? ???? ?????????????????????????? ?? ?????????????? ?????? ???? ????
            auto result  = nontransaction.exec(requestString);

            auto dbElement = result.front();
            fillFields(file, dbElement);

            if (file.type == File::FOLDER)
                if (!checkChildNodes(idString, file, nontransaction))
                    return {};

            nontransaction.commit();

            std::cout << "SELECT request:" << std::endl;
            std::cout << requestString << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Select DB target element was failed" << std::endl;
            std::cout << e.what() << std::endl;
            return {};
        }

        return file;
    }

    std::string DbController::generateUpsertRequest(const File &file) {

        std::string requestString;
        if (file.type == File::FILE)
            requestString = std::string(
                    "INSERT INTO " + TABLE_NAME + " (" + DbFields::ID + ", " + DbFields::PARENT_ID + ", " + DbFields::URL + ", " + DbFields::SIZE + ", " + DbFields::DATE + ") "
                    "VALUES ('" + file.id + "', '" + file.parentId + "', '" + file.url + "', " + std::to_string(file.size) + ", " + std::to_string(file.date) + ") "
                    "ON CONFLICT (" + DbFields::ID + ") DO UPDATE SET "
                    +DbFields::PARENT_ID + " = excluded." + DbFields::PARENT_ID + ", "
                    +DbFields::URL + " = excluded." + DbFields::URL + ", "
                    +DbFields::SIZE + " = excluded." + DbFields::SIZE + ", "
                    +DbFields::DATE + " = excluded." + DbFields::DATE + ";");
        else
            requestString = std::string(
                    "INSERT INTO " + TABLE_NAME + " (" + DbFields::ID + ", " + DbFields::PARENT_ID + ", " + DbFields::SIZE + ", " + DbFields::DATE + ") "
                    "VALUES ('" + file.id + "', '" + file.parentId + "', 0, " + std::to_string(file.date) + ") "
                    "ON CONFLICT (" + DbFields::ID + ") DO UPDATE SET "
                    +DbFields::PARENT_ID + " = excluded." + DbFields::PARENT_ID + ", "
                    +DbFields::SIZE + " = excluded." + DbFields::SIZE + ", "
                    +DbFields::DATE + " = excluded." + DbFields::DATE + ";");

        return requestString;
    }

    std::string DbController::generateDeleteRequest(const std::string& idString) {

        std::string requestString = "WITH RECURSIVE files AS ("
        " SELECT " + DbFields::ID + ", " + DbFields::PARENT_ID+
        " FROM " + TABLE_NAME+
        " WHERE " + DbFields::ID + " = '" + idString + "'"
        " UNION "
        " SELECT " + TABLE_NAME + "." + DbFields::ID + ", " + TABLE_NAME + "." + DbFields::PARENT_ID+
        " FROM " + TABLE_NAME+
        "  JOIN files"
        "   ON " + TABLE_NAME + "." + DbFields::PARENT_ID + " = files." + DbFields::ID+
        ")"
        "DELETE FROM " + TABLE_NAME + " WHERE " + DbFields::ID + " IN (SELECT " + DbFields::ID + " from files);";

        return requestString;
    }

    std::string DbController::generateUpdateParentRequest(const std::string& idString, uint64_t newDate) {
        return "UPDATE " + TABLE_NAME + " set " + DbFields::DATE + " = " + std::to_string(newDate) + " where " + DbFields::ID + " = '" + idString + "';";
    }

    std::string DbController::generateSelectRequest(const std::string& idString) {
        return "SELECT * from " + TABLE_NAME + " where " + DbFields::ID + " = '" + idString + "';";
    }

    std::string DbController::generateSelectChildrenRequest(const std::string& parentIdString) {
        return "SELECT * from " + TABLE_NAME + " where " + DbFields::PARENT_ID + " = '" + parentIdString + "';";
    }

    // ?????????????????????? ?????????? ?? ???? ???????? ???????????????? ??????????????????
    bool DbController::checkChildNodes(const std::string& parentIdString, File& file, pqxx::nontransaction& nontransaction) {

        pqxx::result result;
        try {
            std::string requestString = generateSelectChildrenRequest(parentIdString);
            result = nontransaction.exec(requestString);
        }
        catch (const std::exception& e)
        {
            std::cout << "Select DB child element was failed" << std::endl;
            std::cout << e.what() << std::endl;
            return false;
        }

        //?????????????? ???????? ?????????? ?? ???????????????????????? ???????????????????? ???????? ???????????????????? ?????? ?????????????? ???????????????? ?? ???? ???????????? ???? ??????: ?????????????????? ?????????????? ??????????
        //???? ?????????? ???????????? ???????????????? ?????????????????? ???????????????????? ???????????????? ???????? ?? ???????? ???????? ??????????
        for (pqxx::result::const_iterator dbElement = result.begin(); dbElement != result.end(); ++dbElement) {
            File childNodeFile{};

            fillFields(childNodeFile, dbElement);

            if (childNodeFile.type == File::FOLDER)
                checkChildNodes(childNodeFile.id, childNodeFile, nontransaction);

            file.size += childNodeFile.size;

            if (childNodeFile.date > file.date)
                file.date = childNodeFile.date;

            file.children.push_back(childNodeFile);
        }

        return true;
    }

    void DbController::fillFields(File &file, pqxx::result::reference& dbElement) {
        file.id = dbElement.at(DbFields::ID).as<std::string>();
        file.parentId = dbElement.at(DbFields::PARENT_ID).as<std::string>();
        file.size = dbElement.at(DbFields::SIZE).as<int>();
        file.date = dbElement.at(DbFields::DATE).as<std::uint64_t>();

        if (file.size != 0)
            file.url = dbElement.at(DbFields::URL).as<std::string>();
        //?????? ???????????? ?????????????? ?????? ?????????? ?? ????, ?????????? ?????? ?????????? ?????????????????????? ???????????????????????? ???? ???????? size
        file.type = file.size != 0 ? File::FILE : File::FOLDER;
    }

}
