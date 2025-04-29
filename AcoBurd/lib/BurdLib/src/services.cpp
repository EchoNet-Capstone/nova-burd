#include "service_list.hpp"

#include "services.hpp"

// TODO: create services struct

static Service* allServices[] = {
    // TODO: put services here
};

static constexpr size_t numServices = sizeof(allServices) / sizeof(allServices[0]);

void registerAllServices() {
    auto& reg = ServiceRegistry::instance();
    for (size_t i = 0; i < numServices; ++i) {
        reg.registerService(allServices[i]);
    }
}

// ----- Registry -----

ServiceRegistry& ServiceRegistry::instance() {
    static ServiceRegistry inst;
    return inst;
}

void ServiceRegistry::registerService(Service* s) {
    services_.push_back(s);
}

const std::vector<Service*>& ServiceRegistry::services() const {
    return services_;
}