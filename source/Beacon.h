#ifndef BEACON_H_
#define BEACON_H_

namespace CUES{
    struct rawBeacon {
        char start[9];
        char uuid[16];
        char major[2];
        char minor[2];
        char power[1];
    };
}

#endif
