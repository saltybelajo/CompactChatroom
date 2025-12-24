/* Version from: 15.12.2025*/

#include "s_aux.h"

b_socket *curUsers = NULL;

static char base64_table[]  =  {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

void add_b_socket(int fd, struct sockaddr_in addr) {

    b_socket *sock;

    sock = malloc(sizeof *sock);
    sock->fd = fd;
    sock->addr = (struct sockaddr_in ) addr;
    HASH_ADD_INT(curUsers, fd, (b_socket *) sock);

}

b_socket *find_b_socket(int fd) {

    b_socket *sock;

    HASH_FIND_INT(curUsers, &fd, sock);  /* s: output pointer */
    return sock;

}

void delete_b_socket(b_socket *sock) {

    HASH_DEL(curUsers, sock);  /* user: pointer to deletee */
    free(sock);             /* optional; it's up to you! */

}


int anm_construct_msg(char *allocd, int size_alloc, char *author, char *payload) {

    int authorSize = strnlen(author, AUTHORMLEN) + 1;
    int payloadSize = strnlen(payload, MSGMLEN) + 1;
    if (authorSize > AUTHORMLEN ||                                                  /* check if the sizes are within standards */
        payloadSize > MSGMLEN ||
        *author == 0) 
    {
        return(-1);
    }
                                                                /* the additional bytes are the message size byte, and two \0 */
    uint8_t nOfAdditBytes = 3;                                                          
    uint8_t messageSize = nOfAdditBytes + authorSize + payloadSize;
    if (size_alloc < messageSize) {
        return(-1);
    }

    char *message = malloc(messageSize);
    message[0] = (char) messageSize;
    snprintf(message + 1, messageSize - 1, "%s^%s", author, payload);
    strncpy(allocd, message, size_alloc);
    return(1);

}


int broadcast(struct pollfd *fds, size_t fdsSize, char *parcel, size_t parcelSize) {
    int sendCounter = 0;
    for (int i = 0; i < fdsSize; i++) {
        if (fds[i].fd > 0) {
//            printf("sending a parcel to fds[%d].fd\n", i);
            int n0 = write(fds[i].fd, parcel, parcelSize);
            sendCounter++;
        }
        
    }
    return sendCounter;
}

void startup_text() {
    char *s0 = "CompactChatroom-Server v1.0 on Linux.\n";
    printf("%s", s0);
    printf("\n");
    char *cmdList = "Commands: /online, /clients, /quit.\n";
    printf("%s", cmdList);
    printf("\n");

    uint32_t len0 = strnlen(s0, 1024);
    char *s1 = malloc(len0 - 1);
    memset(s1, '#', len0 - 1);
    printf("%s\n\n", s1);

}

int get_server_specs(char *ip, int ipSize, uint16_t *port, const char * const pathToJson) {

    const cJSON *serveripaddress;
    const cJSON *serverport;

    int fd = open(pathToJson, O_RDONLY, 0644);
    if (fd < 0) {
        printf("get_server_specs(): failed opening the .json file.\n");
        return -1;
    }

    struct stat statOfFd;
    if (fstat(fd, &statOfFd) < 0) {
        printf("get_server_specs(): fstat() issue.\n");
        return -1;
    }

    size_t fileSize = statOfFd.st_size;

    char *am0 = malloc(fileSize);
    char *monitorString = malloc(fileSize); 
    char *am1 = monitorString;
    monitorString = mmap(am0, fileSize, PROT_READ, MAP_SHARED, fd, 0);
    
    close(fd);
    

    if (monitorString == 0) {
        printf("get_server_specs(): monitorString is NULL.\n");
        return(-1);
    }

    cJSON *monitorJson = cJSON_Parse(monitorString);
    if (monitorJson == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        goto end;
    }

    serveripaddress = cJSON_GetObjectItemCaseSensitive(monitorJson, "serveripaddress");
    if (cJSON_IsString(serveripaddress) && (serveripaddress->valuestring != NULL))
    {
        strncpy(ip, serveripaddress->valuestring, ipSize);
    }
    else {
        printf("get_server_specs(): error parsing a string.\n");
        return -1;
    }

    serverport = cJSON_GetObjectItemCaseSensitive(monitorJson, "serverport");
    if (serverport->valueint <= 65535) {
        *port = serverport->valueint;
    }
    else {
        printf("get_server_specs(): error parsing an int.\n");
        return -1;
    }




    end:
        cJSON_Delete(monitorJson);
        free(am0);
        free(am1);
        return 1;

}


void random_base64_string(char *string, size_t stringSize) {
    unsigned long r; 
    int max = 63;
    int min = 0;
    for(int i = 0; i < stringSize; i++) {
        r = ((double) rand() / (RAND_MAX)) * (max-min+1) + min;
        string[i] = base64_table[r];
    }
}

/* int possible_command(char *__string, size_t stringSize) { /* returns -1 if empty string, 0 if no command detected, and 1 if a command detected and executed 

    if (__string == NULL || stringSize == 0) {
        return -1;
    }
    char *string = malloc(stringSize);
    strncpy(string, __string, stringSize);
    char *alloc0 = string;


    if(string[0] != '/') {
        return 0;
    }

    unsigned long hashValue = hash_sdbm(string);
    if (hashValue == hash_sdbm("/quit\n")) {
        exit(EXIT_SUCCESS);
    }
    if (hashValue == hash_sdbm("/stats\n")) {

    }

} */


