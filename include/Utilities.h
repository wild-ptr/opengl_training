#pragma once

#define CALL_ONCE \
    static bool call_once = false; \
    if(!call_once)

#define CALL_ONCE_END call_once = true

// use only on stack defined arrays not decayed to pointers.
#define foreach(item, array) \
    for(int keep=1, \
            count=0,\
            size=sizeof (array)/sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array)+count; keep; keep = !keep)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

#define SWAP(x, y, T) do { T tmp = (x); (x) = (y); (y) = tmp; } while(0)

