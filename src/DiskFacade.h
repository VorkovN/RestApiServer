#pragma once

#include "DbController.h"
#include "HttpTransport.h"

class DiskFacade {

public:
    void start();

private:
    DbController _dbController{};
    HttpTransport _httpTransport{};
};

