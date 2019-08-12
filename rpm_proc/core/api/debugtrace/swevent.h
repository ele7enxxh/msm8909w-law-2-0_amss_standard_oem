#ifndef SWEVENT_H
#define SWEVENT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    //

#include "tracer_event_ids.h"
#include "comdef.h"

#define EXTEND_ARGS(...) \
         EXTEND_ARGS_(0, ##__VA_ARGS__, FILLER_ARGS())
#define EXTEND_ARGS_(...) \
         SELECT_ARGS(__VA_ARGS__)
#define SELECT_ARGS(_ignore, _1, _2, _3, _4, ...) (uint32)_1, (uint32)_2, (uint32)_3, (uint32)_4
#define FILLER_ARGS() 0, 0, 0, 0

void SWEVENT_log(tracer_event_id_t id, uint32 d0, uint32 d1, uint32 d2, uint32 d3);

/* SWEVENT API */
#define SWEVENT(id, ...) SWEVENT_log((id), ##EXTEND_ARGS(__VA_ARGS__))

#ifdef __cplusplus
};
#endif // __cplusplus


#endif // SWEVENT_H

