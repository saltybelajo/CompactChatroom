/* Version from: 30.11.2025*/

#include "c_aux.h"

int isTester = 0;
int isConnected = 0;

int anm_deconstruct_msg(char *parcel, char *allocAuthor, char *allocPayload) {
    
    uint8_t parcelSize = parcel[0];
    char *parcelCopy = malloc(parcelSize);
    strncpy(parcelCopy, parcel, parcelSize);
    char *mallocTarget0 = parcelCopy;
    parcelCopy += 1;

    uint8_t authorSize = 0;
    for (int i = 0; parcelCopy[i] != '^' && i < parcelSize; i++) {
        allocAuthor[i] = parcelCopy[i];
        authorSize++;
    }
    allocAuthor[authorSize] = 0;

    parcelCopy += authorSize + 1;
    uint8_t payloadSize = 0;
    for (int i = 0; parcelCopy[i] != 0 && i < parcelSize; i++) {
        allocPayload[i] = parcelCopy[i];
        authorSize++;
    }
    free(mallocTarget0);

    return 1;
}

void startup_text() {
    char *s0 = "CompactChatroom-Client v1.0 on Linux.\n";
    printf("%s", s0);
    printf("\n");

    uint32_t len0 = strnlen(s0, 1024);
    char *s1 = malloc(len0 - 1);
    memset(s1, '#', len0 - 1);
    printf("%s\n\n", s1);

}