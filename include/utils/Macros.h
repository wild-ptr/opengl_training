#pragma once

#define RF_ZERO_STRUCT(type, ptr) \
    memset((ptr), 0, sizeof(Type))

// add the trick with __init symbol gluing together
#define RF_INIT_CTOR(type, ptr) \

