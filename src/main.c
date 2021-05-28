#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>

#define PORT    1981 //8080
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

int analyzeData(uint8_t xorkey, char buffer[], uint16_t frame, uint16_t input[],
        uint16_t SEQ, uint16_t numberOfObjetcs, unsigned char decriptedString[])
{
    printf("\nCurrent Frame 0x%02X\n", frame >> 1);
    printf("\nXorkey: 0x%02X - Buffer[0]: 0x%02x\n", xorkey, buffer[0]);
    printf("Input 0x%02X\n", input[0] | input[1]);
    printf("SEQ 0x%02X\n", SEQ);
    printf("Number Of Objects 0x%02X\n\n", numberOfObjetcs);

    if (numberOfObjetcs > 0x00)
    {
        for (int i = 1 ; i <= numberOfObjetcs ; i++)
        {
            printf("\tObject: 0x%02X\n", i);
            printf("\t\tType: 0x%02X\n", decriptedString[i * 3]);
            printf("\t\tH position: 0x%02X\n", decriptedString[(i * 3) + 1]);
            printf("\t\tV position: 0x%02X\n", decriptedString[(i * 3) + 2]);
        }
        printf("\n\n");
        return 0;
    }
    else
        return 1;
}

char* decryptString(char inpString[], const int len, uint8_t xorKey)
{
    printf("decryptString: ");
    for (int i = 0; i < len; i++)
    {
        inpString[i] = inpString[i] ^ xorKey;
        /*if(i % 10 == 0)*/
        /*    printf("\n");*/
        /*printf("0x%02X ",inpString[i]);*/
    }
    /*printf("\n");*/
    hex_print((unsigned char *)inpString, len);

    return inpString;
}

int main() {
    int sockfd;
    char buffer[MAXLINE];
    uint8_t inputPackage[2] = {0x00, 0x00};
    struct sockaddr_in     servaddr;


    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("18.219.219.134"); //INADDR_ANY;

    unsigned int n, len;

    sendto(sockfd, &inputPackage, sizeof(inputPackage),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));
    printf("Message sent to server...\n\n");

    n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);


    printf("Received message with size: %d\n", n);
    printf("Server message: ");
    hex_print((unsigned char *)buffer, n);

    uint8_t xorkey = buffer[0];

    unsigned char *decriptedString ;
    decriptedString = (unsigned char *) decryptString(buffer, n, xorkey);

    uint16_t frame  = decriptedString[0] & 0xFE;
    uint16_t input[2] = {decriptedString[0] & 0x01, (decriptedString[1] & 0x80) >> 6};
    uint16_t SEQ  = decriptedString[1] & 0x7F;
    uint16_t numberOfObjetcs  = decriptedString[2];

    int result = analyzeData(xorkey, buffer, frame, input, SEQ, numberOfObjetcs, decriptedString);
    printf("\n\nResult: %d\n\n", result);

    inputPackage[0] = ((frame + 0x01) << 1) + 0x01;
    inputPackage[1] = SEQ + 0x80;

    printf("%02x%02x\n", inputPackage[0], inputPackage[1]);

    memset(&buffer, 0, n);

    sendto(sockfd, &inputPackage, sizeof(inputPackage),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));
    printf("Message sent to server...\n\n");

    n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);

    printf("Received message with size: %d\n", n);
    printf("Server message: ");
    hex_print((unsigned char *)buffer, n);

    xorkey = buffer[0] ^ (inputPackage[0]);
    decriptedString = (unsigned char *) decryptString(buffer, n, xorkey);

    frame  = decriptedString[0] & 0xFE;
    input[0] = decriptedString[0] & 0x01;
    input[1] = (decriptedString[1] & 0x80) >> 6;
    SEQ  = decriptedString[1] & 0x7F;
    numberOfObjetcs  = decriptedString[2];

    result = analyzeData(xorkey, buffer, frame, input, SEQ, numberOfObjetcs, decriptedString);
    printf("\n\nResult: %d\n\n", result);

    inputPackage[0] = (((frame >> 1) + 0x01) << 1) + 0x01;
    inputPackage[1] = SEQ + 0x80;

    memset(&buffer, 0, n);

    sendto(sockfd, &inputPackage, sizeof(inputPackage),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));
    printf("Message sent to server...\n\n");

    n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);

    printf("Received message with size: %d\n", n);
    printf("Server message: ");
    hex_print((unsigned char *)buffer, n);

    xorkey = buffer[0] ^ (inputPackage[0]);
    decriptedString = (unsigned char *) decryptString(buffer, n, xorkey);

    frame  = decriptedString[0] & 0xFE;
    input[0] = decriptedString[0] & 0x01;
    input[1] = (decriptedString[1] & 0x80) >> 6;
    SEQ  = decriptedString[1] & 0x7F;
    numberOfObjetcs  = decriptedString[2];

    result = analyzeData(xorkey, buffer, frame, input, SEQ, numberOfObjetcs, decriptedString);
    printf("\n\nResult: %d\n\n", result);

    close(sockfd);

    return 0;
}
