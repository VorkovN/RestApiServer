#pragma once

#include <optional>

#include <pqxx/pqxx>

#include "File.h"


namespace yandex_disk {

    class DbController {
    public:
        DbController();
        ~DbController();

        bool initialize();

        bool postNode(const File& file);
        bool deleteNode(const std::string& idString, uint64_t newDate);
        std::optional<File> getNode(const std::string& idString);

    private:
        std::string generateUpsertRequest(const File& file);
        std::string generateDeleteRequest(const std::string& idString);
        std::string generateUpdateParentRequest(const std::string& idString, uint64_t newDate);
        std::string generateSelectRequest(const std::string& idString);
        std::string generateSelectChildrenRequest(const std::string& idString);
        bool checkChildNodes(const std::string& idString, File &file, pqxx::nontransaction& nontransaction);
        void fillFields(File &file, pqxx::result::reference& dbElement);

    private:
        std::unique_ptr<pqxx::connection> _dbConnection;
        const std::string USER_NAME = "vorkov";
        const std::string DB_NAME = "disk";
        const std::string TABLE_NAME = "YandexDisk";
        const std::string PASSWORD = "pass";
        const std::string ADDRESS = "127.0.0.1";
        const std::string PORT = "5432";
        const std::string CONNECTTION_REQUEST = "user = " + USER_NAME + " dbname = " + DB_NAME + " password = " + PASSWORD + " hostaddr = " + ADDRESS + " port = " + PORT;

    private:
    struct DbFields{
        inline static const std::string ID = "id";
        inline static const std::string URL = "url";
        inline static const std::string TYPE = "type";
        inline static const std::string PARENT_ID = "parent_id";
        inline static const std::string DATE = "date";
        inline static const std::string SIZE = "size";
        inline static const std::string CHILDREN = "children";
    };
    };

}


