/* Version from: 30.11.2025*/

#include "s_aux.h"

int writeft(int fd, char *msgbuf, char *actor)
{
    char datetime_str[128];

    if (strnlen(msgbuf, MSGMLEN) > MSGMLEN)
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

    if (strftime(datetime_str, sizeof(datetime_str), "%b %d %T", tmp) == 0) {
        perror("writetf: strftime");
        return(-1);
    }

    char finoutput[1024];
    memset(finoutput, '\0', sizeof(finoutput));
    snprintf(finoutput, sizeof(finoutput) - 1, "%s %s: %s", datetime_str, actor, msgbuf);
    write(fd, finoutput, sizeof(finoutput));



    return(strnlen(finoutput, 1024));
}
