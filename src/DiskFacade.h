#pragma once

#include "DbController.h"
#include "HttpTransport.h"

namespace yandex_disk {

    class DiskFacade {

    public:
        void start();

    private:
        DbController _dbController{};
        HttpTransport _httpTransport{};
    };

}
