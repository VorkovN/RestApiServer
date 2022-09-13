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

    bool DiskFacade::deleteNode() {
        return _dbController.deleteNode();
    }

    File DiskFacade::getNode() {
        return _dbController.getNode();
    }

}
