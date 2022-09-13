#pragma once

#include "File.h"
#include "IDiskFacade.h"


namespace yandex_disk {

    class DiskFacade: public IDiskFacade {

    public:
        void start() override;
        bool postNode(File& file) override;
        bool deleteNode() override;
        File getNode() override;

    private:
        DbController _dbController{};
        HttpTransport _httpTransport{};
    };

}
