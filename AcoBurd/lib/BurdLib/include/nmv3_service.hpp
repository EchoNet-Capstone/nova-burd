#pragma once

void
modemService(
    void
);

#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
void
nestSerialService(
    void
);
#endif

void
nmv3_init(
    void
);