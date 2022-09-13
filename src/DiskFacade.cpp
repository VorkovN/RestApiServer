#include "DiskFacade.h"

namespace yandex_disk {


    void DiskFacade::start() {
        _httpTransport.start();
    }

}
