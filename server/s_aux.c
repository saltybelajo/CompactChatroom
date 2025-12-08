/* Version from: 30.11.2025*/

#include "s_aux_v01_01.h"

u_int16_t MSGMLEN = 512;

int writeft(int fd, char *msgbuf, char *actor)
{
    char datetime_str[128];
    char space_ch = ' ';
    char colon_ch = ':';

    if (sizeof(msgbuf) > MSGMLEN)
    {
        perror("writeft: msgbuf out of bounds");
        return(-1);
    }    
    
    time_t t = time(NULL);
    struct tm *tmp = localtime(&t);
    
    if (tmp == NULL) {
        perror("writeft: tmp");
        return(-1);
    }

    if (strftime(datetime_str, sizeof(datetime_str), "%b %d %T ", tmp) == 0) {
        perror("writetf: strftime");
        return(-1);
    }

    char finoutput[1024];
    memset(finoutput, '\0', sizeof(finoutput));

    uint16_t curelem = 0;

    strncat(finoutput, datetime_str, strlen(datetime_str)); // Dec 1 17:25:30\0
    curelem += strlen(datetime_str);

    strncat(finoutput, actor, strlen(actor)); // Dec 1 17:25:30 192.168.0.18\0
    curelem += strlen(actor);

    finoutput[curelem] = colon_ch; // Dec 1 17:25:30 192.168.0.18:\0
    curelem += sizeof(char);

    finoutput[curelem] = space_ch; // Dec 1 17:25:30 192.168.0.18: \0
    curelem += sizeof(char);

    strncat(finoutput, msgbuf, strlen(msgbuf));
    curelem += strlen(msgbuf);


    write(fd, finoutput, sizeof(finoutput));
    return(strlen(finoutput));
    
}
