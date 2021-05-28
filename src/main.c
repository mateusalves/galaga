#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>

#define PORT    1981
#define MAXLINE 1024

int hex_print(unsigned char *in, size_t len)
{
    // in can't be NULL
    if(in == NULL)
        return -1;

    // len must have at least 1 byte
    if(len <= 0)
        return -2;

    for(int i = 0; i < len; i++) {
        if(i % 10 == 0)
            printf("\n");
        printf("0x%02X ", *(in + i));
    }
    printf("\n");
    return 0;
}

int analyzeData(uint8_t xorkey, char buffer[], unsigned int n,  uint16_t frame, uint16_t input[],
        uint16_t SEQ, uint16_t numberOfObjetcs, unsigned char decriptedString[])
{
    printf("\nCurrent Frame %d\n", frame >> 1);
    printf("Input %d\n", input[0] | input[1]);
    /*printf("SEQ 0x%02X\n", SEQ);*/
    printf("Number Of Objects %d\n\n", numberOfObjetcs);

    double objectsData = n - 3.0;
    int isMyShipIntact = 0;

    if (numberOfObjetcs > 0x00)
    {
        if(objectsData / numberOfObjetcs < 1.0)
             return -1;

        for (int i = 1 ; i <= numberOfObjetcs ; i++)
        {
            printf("\tObject: %d\n", i);
            printf("\t\tType: %d\n", decriptedString[i * 3]);

            if (decriptedString[i * 3] == 0x00)
                isMyShipIntact = 1;

            printf("\t\tH position: %d\n", decriptedString[(i * 3) + 1]);
            printf("\t\tV position: %d\n", decriptedString[(i * 3) + 2]);
        }
        printf("\n\n");
        return isMyShipIntact;
    }
    else
        return -1;
}

char* decryptString(char inpString[], const int len, uint8_t xorKey)
{
    printf("Decrypted String: ");
    for (int i = 0; i < len; i++)
        inpString[i] = inpString[i] ^ xorKey;

    hex_print((unsigned char *)inpString, len);
    return inpString;
}


int main() {
    int sockfd;
    char buffer[MAXLINE];
    uint8_t inputPackage[2] = {0x00, 0x00};
    struct sockaddr_in     servaddr;

    struct timeval timeout;
    timeout.tv_usec = 500000;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt failed\n");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("18.219.219.134");

    unsigned int n, len;
    uint8_t xorkey;
    unsigned char *decriptedString ;

    uint16_t frame = 0x00;
    uint16_t input[2] = {0x00, 0x00};
    uint16_t SEQ;
    uint16_t numberOfObjetcs;

    int result;

    while(frame != 0xFE)
    {
        sendto(sockfd, &inputPackage, sizeof(inputPackage),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));
        printf("Message sent to server...\n\n");

        n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                    MSG_WAITALL, (struct sockaddr *) &servaddr,
                    &len);

        printf("\n\nn : %d\n\n", n);

        if (n == -1)
            continue;

        printf("Received message with size: %d\n", n);
        printf("Server message: ");
        hex_print((unsigned char *)buffer, n);

        xorkey = buffer[0] ^ inputPackage[0];

        decriptedString = (unsigned char *) decryptString(buffer, n, xorkey);

        frame  = decriptedString[0] & 0xFE;
        input[0] = decriptedString[0] & 0x01;
        input[1] = (decriptedString[1] & 0x80) >> 6;
        SEQ = decriptedString[1] & 0x7F;
        numberOfObjetcs  = decriptedString[2];

        result = analyzeData(xorkey, buffer, n, frame, input, SEQ, numberOfObjetcs, decriptedString);
        printf("\n\n\tResult: %d\n\n", result);

        if(result == 0)
        {
            printf("\n\n\t********** GAME OVER **********\n\n");
            break;
        }
        else if(result == -1)
            continue;

        inputPackage[0] = (((frame >> 1) + 0x01) << 1) + 0x01;
        inputPackage[1] = SEQ + 0x80;
        memset(&buffer, 0, n);

        printf("%02x%02x\n", inputPackage[0], inputPackage[1]);
    }
    close(sockfd);

    return 0;
}
