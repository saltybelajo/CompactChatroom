/* Version from: 15.12.2025*/

#include "s_aux.h"

b_socket *curUsers = NULL;



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

    uint32_t len0 = strnlen(s0, 1024);
    char *s1 = malloc(len0 - 1);
    memset(s1, '#', len0 - 1);
    printf("%s\n\n", s1);

}

int get_server_specs(char *ip, int ipSize, uint16_t *port, int portSize, const char * const pathToJson) {

    const cJSON *serveripaddress;
    const cJSON *serverport;

    int fd = open(pathToJson, O_RDONLY, 0644);
    if (fd < 0) {
        printf("get_server_specs(): failed opening the .json file.\n");
        return -1;
    }

    char *monitorString = malloc(1024);
    char a = 'a';

    for (int i = 0; a != 0; i++) {
        int r0 = read(fd, &a, 1);
        if (r0 == -1) {
            printf("get_server_specs(): read returned -1.");
            return -1;
        }
        monitorString[i] = a;
    }

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
    
    if (sizeof(serverport->valueint) <= portSize) {
        *port = serverport->valueint;
    }
    else {
        printf("get_server_specs(): error parsing an int.\n");
        return -1;
    }




    end:
        cJSON_Delete(monitorJson);
        return 1;

}


