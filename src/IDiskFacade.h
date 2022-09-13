#pragma once

#include "File.h"


namespace yandex_disk {

    class IDiskFacade {

    public:
        virtual void start() = 0;
        virtual bool postNode(const File& file) = 0;
        virtual bool deleteNode() = 0;
        virtual File getNode() = 0;

        virtual ~IDiskFacade() noexcept = default;
    };

}