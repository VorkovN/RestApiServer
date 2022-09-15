#pragma once

#include <optional>

#include "File.h"


namespace yandex_disk {

    class IDiskFacade {

    public:
        virtual void start() = 0;
        virtual bool postNode(const File& file) = 0;
        virtual bool deleteNode(const std::string& idString, uint64_t newDate) = 0;
        virtual std::optional<File> getNode(const std::string& idString) = 0;

        virtual ~IDiskFacade() noexcept = default;
    };

}