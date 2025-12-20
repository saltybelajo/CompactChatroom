/* Version from: 08.12.2025*/

#include "common.h"

int writeft(int fd, char *msgbuf, char *author)
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
    snprintf(finoutput, sizeof(finoutput) - 1, "%s %s: %s", datetime_str, author, msgbuf);
    write(fd, finoutput, sizeof(finoutput));



    return(strnlen(finoutput, 1024));

}

unsigned long sdbm(char *str) {

        unsigned long hash = 0;
        int c;

        while (c = *str++)
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;

}

int getlsocket(char *ipAddr, uint16_t port) {

    int fd;
    struct sockaddr_in addr;



    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int s = inet_pton(AF_INET, ipAddr, (struct sockaddr *) &addr.sin_addr.s_addr);
    if (s <= 0) {
        return(-1);
    }

    size_t size = sizeof(addr);
    int n = bind(fd, (struct sockaddr *) &addr, size);
    if (n == -1) {
        return(-1);
    } 

    if ((n = listen(fd, CLIENTCAP)) == -1) {
        return(-1);
    }


    return(fd);




}

unsigned long closest_pow2(int v) {
    if (v == 0) 
        return 1;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return(v);
}

int write_to_user(int fd, char *msgbuf, char *author)
{
    char datetime_str[128];

    if (strnlen(msgbuf, MSGMLEN) > MSGMLEN)
    {
        perror("write_to_user(): msgbuf out of bounds");
        return(-1);
    }    

    time_t t = time(NULL);
    struct tm *tmp = localtime(&t);
    
    if (tmp == NULL) {
        perror("write_to_user(): tmp");
        return(-1);
    }

    if (strftime(datetime_str, sizeof(datetime_str), "%H:%M", tmp) == 0) {
        perror("write_to_user(): strftime");
        return(-1);
    }

    char finoutput[1024];
    memset(finoutput, '\0', sizeof(finoutput));
    snprintf(finoutput, sizeof(finoutput) - 1, "%s %s: %s", datetime_str, author, msgbuf);
    write(fd, finoutput, sizeof(finoutput));



    return(strnlen(finoutput, 1024));

}

char *resolve_my_ip_address() {

    char hostBuffer[256];
    char *IPBuffer;
    struct hostent *hostEntry;
    int hostName;

    hostName = gethostname(hostBuffer, 256);
    hostEntry = gethostbyname(hostBuffer);
    //inet_ntop(AF_INET, (struct in_addr *) hostEntry->h_addr_list[0], IPBuffer, 256);
    IPBuffer = inet_ntoa(*((struct in_addr*) hostEntry->h_addr_list[0]));

    return IPBuffer;

}

/* char *truncate(char *str, unsigned int size) {

    if (strnlen(str, size) > size - 1) {
        char *newstr;
        strncpy(newstr, str, size);
        return(newstr);
    }
    else
        return(str);

} */