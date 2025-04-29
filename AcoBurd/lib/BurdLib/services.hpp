#pragma once

#include <vector>
#include <cstdint>

/**
 * A single service descriptor:
 *  - fn:     the work-slice function to run
 *  - period: how often to run, in milliseconds (0 = every loop)
 *  - lastRun: last millis() timestamp it ran
 *  - busy:   set to true by fn() if it actually did work
 */
struct Service {
    using Fn = void(*)();
    Fn       fn;
    uint32_t period;
    uint32_t lastRun;
    bool     busy;
};

/**
 * A singleton registry of Service* pointers.
 * Services call registerService() at runtime (e.g. in setup()).
 * The scheduler pulls the list via services().
 */
class ServiceRegistry {
public:
    // Get the one and only registry instance
    static ServiceRegistry& instance();

    // Add one service descriptor
    void registerService(Service* s);

    // Fetch the list for scheduling
    const std::vector<Service*>& services() const;

private:
    ServiceRegistry() = default;
    ServiceRegistry(const ServiceRegistry&) = delete;
    ServiceRegistry& operator=(const ServiceRegistry&) = delete;

    std::vector<Service*> services_;
};

/**
* Call this (once) in setup() to wire up every service.
*/
void registerAllServices();