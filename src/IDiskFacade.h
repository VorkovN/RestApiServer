#pragma once

#include "File.h"
#include "DbController.h"
#include "HttpTransport.h"

namespace yandex_disk {

    class IDiskFacade {

    public:
        virtual void start() = 0;

        virtual bool postNode(File &file) = 0;

        virtual bool deleteNode() = 0;

        virtual File getNode() = 0;

    };

}