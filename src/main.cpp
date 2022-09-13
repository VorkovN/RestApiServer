#include "DiskFacade.h"

int main() {

    yandex_disk::DiskFacade diskFacade{};
    diskFacade.start();

    return 0;
}
