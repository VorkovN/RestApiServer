#include "DiskFacade.h"

namespace yandex_disk {

    void DiskFacade::start() {

        if (!_dbController.initialize())
            return;

        _httpTransport.initialize(this);
        _httpTransport.start();
    }

    bool DiskFacade::postNode(const File& file) {
        return _dbController.postNode(file);
    }

    bool DiskFacade::deleteNode(const std::string& idString, uint64_t newDate) {
        return _dbController.deleteNode(idString, newDate);
    }

    std::optional<File> DiskFacade::getNode(const std::string& idString) {
        return _dbController.getNode(idString);
    }

}
