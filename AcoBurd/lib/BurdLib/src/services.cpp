#include "safe_arduino.hpp"

#include "service_list.hpp"

#include "services.hpp"

// TODO: create services structs
/**
Service myServiceDesc = {
    myServiceTask,
    period (interval in ms),
    0,
    false
}
 */
Service motorServiceDesc = {
    motorService,
    0,
    0,
    false
};

Service activityServiceDesc = {
    activityService,
    0,
    0,
    false
};

Service bufferServiceDesc = {
    bufferService,
    0,
    0,
    false
};

Service modemServiceDesc = {
    modemService,
    0,
    0,
    false
};

// Service neighborServiceDesc = {
//     neighborService,
//     0,
//     0,
//     false
// };

#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
Service nestSerialServiceDesc = {
    nestSerialService,
    0,
    0,
    false
};
#endif // RECV_SERIAL_NEST

static Service* allServices[] = {
    // TODO: put services here
    // &myServiceDesc,
#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
    &nestSerialServiceDesc,
#endif // RECV_SERIAL_NEST

    &modemServiceDesc,
    &activityServiceDesc,
    &bufferServiceDesc,
    &motorServiceDesc,
    // &neighborServiceDesc,
};

static constexpr size_t numServices = sizeof(allServices) / sizeof(allServices[0]);

void 
registerAllServices(
    void
) {
    auto& reg = ServiceRegistry::instance();
    for (size_t i = 0; i < numServices; ++i) {
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Registering service %p...\r\n", allServices[i]);
    #endif
        reg.registerService(allServices[i]);
    }
}

// ----- Registry -----

ServiceRegistry& 
ServiceRegistry::instance(

) {
    static ServiceRegistry inst;
    return inst;
}

void 
ServiceRegistry::registerService(
    Service* s
) {
    services_.push_back(s);
}

const 
std::vector<Service*>& ServiceRegistry::services(
    void
) const {
    return services_;
}