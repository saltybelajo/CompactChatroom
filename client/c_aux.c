/* Version from: 30.11.2025*/

#include "c_aux.h"

int anm_receive_msg(char *parcel, char *allocActor, char *allocMsg) {
    int actorBits = parcel[0] - '0';
    int msgBits = parcel[1] - '0';

    size_t actorSize = (int)pow(2, actorBits);
    size_t msgSize = (int)pow(2, msgBits);

    char *parcel2 = parcel + 2;
    char *parcel3 = parcel2 + actorSize;

    strncpy(allocActor, parcel2, actorSize);
    strncpy(allocMsg, parcel3, msgSize);

    return(1);
}