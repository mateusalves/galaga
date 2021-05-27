// Client side implementation of UDP client-server model
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


char* encryptDecrypt(char inpString[], const int len, uint8_t xorKey)
{
    printf("encryptDecrypt: ");
    for (int i = 0; i < len; i++)
    {
        inpString[i] = inpString[i] ^ xorKey;
        printf("%02X ",inpString[i]);
    }
    printf("\n");
    return inpString;
}

int main() {
    int sockfd;
    char buffer[MAXLINE];
    /*int16_t inputPackage = 0b0000000100000000;*/
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
    printf("Hello message sent.\n");

    n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);


    printf("Len recvfrom: %d\n", n);
    printf("Server: ");
    for (int i = 0; i < n; i++)
        printf("%02X ", buffer[i]);
    printf("\n");

    /*uint8_t xorkey = buffer[0];*/
    uint8_t xorkey = buffer[0];

    printf("\n\n%02x - %02x\n\n", xorkey, buffer[0]);
    unsigned char *decriptedString ;
    decriptedString = (unsigned char *) encryptDecrypt(buffer, n, xorkey);

    for (int i = 0; i < n; i++)
        printf("%02X ", decriptedString[i]);
    printf("\n");

    uint16_t frame  = decriptedString[0] & 0xFE;
    uint16_t input[2] = {decriptedString[0] & 0x01, decriptedString[1] & 0x80};
    uint16_t SEQ  = decriptedString[1] & 0x7F;
    uint16_t numberOfObjetcs  = decriptedString[2];
    /*uint16_t number  = decriptedString[1] & 0xFF;*/

    printf("Frame %02X\n", frame);
    printf("Frame + 1 %02X\n", frame + 1);
    printf("Input %X%X\n", input[0], input[1]);
    printf("SEQ %02X\n", SEQ);
    printf("Number Of Objects %02X\n", numberOfObjetcs);

    printf("Frame %02X\n", frame);
    uint16_t frameAux = frame;
    uint16_t seqAux = SEQ;
    printf("calculus A %02X\n", ((frameAux + 0x01) << 1) + 0x00);
    printf("calculus B %02X\n", seqAux + 0x00);


    inputPackage[0] = ((frame + 0x01) << 1) + 0x01;
    inputPackage[1] = SEQ + 0x80;

    printf("%02x%02x\n", inputPackage[0], inputPackage[1]);


    memset(&buffer, 0, n);

    sendto(sockfd, &inputPackage, sizeof(inputPackage),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));
    printf("Hello message sent.\n");

    n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);


    printf("Len recvfrom: %d\n", n);
  printf("Server: ");
    for (int i = 0; i < n; i++)
        printf("%02X ", buffer[i]);
    printf("\n");


    xorkey = buffer[0] ^ (inputPackage[0]);
printf("\n\n%02x - %02x\n\n", xorkey, buffer[0]);
    decriptedString = (unsigned char *) encryptDecrypt(buffer, n, xorkey);

    for (int i = 0; i < n; i++)
        printf("%02X ", decriptedString[i]);
    printf("\n");


    frame  = decriptedString[0] & 0xFE;
    input[0] = decriptedString[0] & 0x01;
    input[1] = (decriptedString[1] & 0x80) >> 6;
    SEQ  = decriptedString[1] & 0x7F;
    numberOfObjetcs  = decriptedString[2];


    printf("Frame %02X\n", frame);
    printf("Input %X\n", input[0] | input[1]);
    printf("SEQ %02X\n", SEQ);
    printf("Number Of Objects %02X\n", numberOfObjetcs);

    /*FRAME 2 ====================*/

    frameAux = frame;
    seqAux = SEQ;
    printf("calculus A %02X\n", ((frameAux + 0x01) << 1) + 0x01);
    printf("calculus B %02X\n", seqAux + 0x80);


    inputPackage[0] = (((frame >> 1) + 0x01) << 1) + 0x01;
    inputPackage[1] = SEQ + 0x80;

    printf("%02x%02x\n", inputPackage[0], inputPackage[1]);


    memset(&buffer, 0, n);

    sendto(sockfd, &inputPackage, sizeof(inputPackage),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));
    printf("Hello message sent.\n");

    n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);


    printf("Len recvfrom: %d\n", n);
  printf("Server: ");
    for (int i = 0; i < n; i++)
        printf("%02X ", buffer[i]);
    printf("\n");


    xorkey = buffer[0] ^ (inputPackage[0]);
printf("\n\n%02x - %02x\n\n", xorkey, buffer[0]);
    decriptedString = (unsigned char *) encryptDecrypt(buffer, n, xorkey);

    for (int i = 0; i < n; i++)
        printf("%02X ", decriptedString[i]);
    printf("\n");


    frame  = decriptedString[0] & 0xFE;
    input[0] = decriptedString[0] & 0x01;
    input[1] = decriptedString[1] & 0x80;
    SEQ  = decriptedString[1] & 0x7F;
    numberOfObjetcs  = decriptedString[2];


    printf("Frame %02X\n", frame);
    printf("Frame + 1 %02X\n", frame + 1);
    printf("Input %X%X\n", input[0], input[1]);
    printf("SEQ %02X\n", SEQ);
    printf("Number Of Objects %02X\n", numberOfObjetcs);


    close(sockfd);

    return 0;
}
