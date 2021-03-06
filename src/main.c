#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include "log.h"

#define IP_ADDR "18.219.219.134"
#define PORT    1981
#define MAXLINE 1024
#define LASTFRAME 0xFE

enum State {ERROR_ON_SHIP_SYSTEM = -1, SHIP_DESTROYED, SHIP_INTACT};

typedef struct{
    int enemysProjectiles;
    int *h_pos_enemyProjec;
    int *v_pos_enemyProjec;
    int enemys;
    int *h_pos_enemys;
    int *v_pos_enemys;
}ObjectsData;

typedef struct{
    int status;
    uint8_t type;
    uint8_t x_coordinate;
    uint8_t y_coordinate;
    uint8_t nextMoveA;
    uint8_t nextMoveB;
}ShipData;

void hex_print(unsigned char *in, size_t len)
{
    // in can't be NULL
    if(in == NULL)
        return;

    // len must have at least 1 byte
    if(len <= 0)
        return;

    for(int i = 0; i < len; i++) {
        if(i % 10 == 0)
            log_message("\n");
        log_message("0x%02X ", *(in + i));
    }
    log_message("\n");
    return;
}

void nextMove(int numberOfObjetcs, ObjectsData *objsInFrame, ShipData *ship)
{
    for (int i = 0; i < objsInFrame->enemysProjectiles; i++)
    {
        if (objsInFrame->h_pos_enemyProjec[i] == ship->x_coordinate)
        {
            if (objsInFrame->v_pos_enemyProjec[i] == 0x01)
            {
                int goLeft = 0; int goRight = 0;
                for (int j = 0; j < objsInFrame->enemysProjectiles; j++)
                {
                    if (objsInFrame->h_pos_enemyProjec[i] < 0x08 &&
                            objsInFrame->h_pos_enemyProjec[j] == objsInFrame->h_pos_enemyProjec[i] + 1)
                    {
                        if (objsInFrame->v_pos_enemyProjec[j] == 0x01)
                            goLeft++;
                    }
                    if (objsInFrame->h_pos_enemyProjec[i] > 0x00 &&
                            objsInFrame->h_pos_enemyProjec[j] == objsInFrame->h_pos_enemyProjec[i] - 1)
                    {
                        if (objsInFrame->v_pos_enemyProjec[j] == 0x01)
                            goRight++;
                    }
                    if (ship->x_coordinate == 0x08)
                    {
                        //Move right
                        ship->nextMoveA = 0x00;
                        ship->nextMoveB = 0x80;
                    }
                    if (ship->x_coordinate == 0x00)
                    {
                        //Move left
                        ship->nextMoveA = 0x01;
                        ship->nextMoveB = 0x00;
                    }
                }

                if (goRight == 0)
                {
                    //Move right
                    ship->nextMoveA = 0x00;
                    ship->nextMoveB = 0x80;
                }
                else if (goLeft == 0)
                {
                    //Move left
                    ship->nextMoveA = 0x01;
                    ship->nextMoveB = 0x00;
                }
                 return;
            }
        }
    }

    for (int i = 0; i < objsInFrame->enemys; i++)
    {
        if (objsInFrame->h_pos_enemys[i] == ship->x_coordinate ||
            objsInFrame->h_pos_enemys[i] == (ship->x_coordinate + 0x01) ||
            objsInFrame->h_pos_enemys[i] == (ship->x_coordinate - 0x01))
            {
                //Shoot
                ship->nextMoveA = 0x01;
                ship->nextMoveB = 0x80;
            }
         else
            {
                //Dont move
                ship->nextMoveA = 0x00;
                ship->nextMoveB = 0x00;
            }
    }
    return;
}

ShipData analyzeData(uint8_t xorkey, char buffer[], unsigned int n,  uint16_t frame, uint16_t input[],
        uint16_t SEQ, uint16_t numberOfObjetcs, unsigned char decriptedString[])
{
    int objectsData = n - 3; //Amount of data related to the objects: type, vertical and horizontal positions
    ShipData ship;
    ship.status = SHIP_DESTROYED; //Assuming it was destroyed until we receive the right status

    ObjectsData objsInFrame;
    objsInFrame.enemys = 0;
    objsInFrame.enemysProjectiles = 0;
    objsInFrame.h_pos_enemyProjec = malloc(sizeof(int)*numberOfObjetcs);
    objsInFrame.v_pos_enemyProjec = malloc(sizeof(int)*numberOfObjetcs);
    objsInFrame.h_pos_enemys = malloc(sizeof(int)*numberOfObjetcs);
    objsInFrame.v_pos_enemys = malloc(sizeof(int)*numberOfObjetcs);

    log_message("\nCurrent Frame %d\n", frame >> 1);
    log_message("Xorkey: 0x%02X\n", xorkey);
    log_message("Input %d\n", input[0] | input[1]);
    log_message("SEQ 0x%02X\n", SEQ);
    log_message("Number Of Objects %d\n\n", numberOfObjetcs);

    if (numberOfObjetcs > 0x00)
    {
        if(objectsData / numberOfObjetcs < 1)
        {
            log_message("\nWrong data...\n");
            ship.status = ERROR_ON_SHIP_SYSTEM;
            free(objsInFrame.h_pos_enemyProjec);
            free(objsInFrame.v_pos_enemyProjec);
            free(objsInFrame.h_pos_enemys);
            free(objsInFrame.v_pos_enemys);
            return ship;
        }

        for (int i = 1 ; i <= numberOfObjetcs ; i++)
        {
            log_message("\tObject: %d\n", i);
            log_message("\t\tType: %d\n", decriptedString[i * 3]);
            log_message("\t\tH position: %d\n", decriptedString[(i * 3) + 1]);
            log_message("\t\tV position: %d\n", decriptedString[(i * 3) + 2]);

            if (decriptedString[i * 3] == 0x00)
            {
                ship.status = SHIP_INTACT;
                ship.type = 0x00;
                ship.x_coordinate = decriptedString[(i * 3) + 1];
                ship.y_coordinate = decriptedString[(i * 3) + 2];
            }
            else if (decriptedString[i * 3] == 0x02)
            {
                *(objsInFrame.h_pos_enemys + objsInFrame.enemys) = decriptedString[(i * 3) + 1];
                *(objsInFrame.v_pos_enemys + objsInFrame.enemys) = decriptedString[(i * 3) + 2];
                objsInFrame.enemys++;

            }
            else if (decriptedString[i * 3] == 0x03)
            {
                *(objsInFrame.h_pos_enemyProjec + objsInFrame.enemysProjectiles) = decriptedString[(i * 3) + 1];
                *(objsInFrame.v_pos_enemyProjec + objsInFrame.enemysProjectiles) = decriptedString[(i * 3) + 2];
                objsInFrame.enemysProjectiles++;
            }
        }
        log_message("\n\n");

        if (ship.status != SHIP_DESTROYED)
            nextMove(numberOfObjetcs, &objsInFrame, &ship);
    }
    else
        ship.status = ERROR_ON_SHIP_SYSTEM;

    free(objsInFrame.h_pos_enemyProjec);
    free(objsInFrame.v_pos_enemyProjec);
    free(objsInFrame.h_pos_enemys);
    free(objsInFrame.v_pos_enemys);
    return ship;
}

char* decryptString(char inpString[], const int len, uint8_t xorKey)
{
    log_message("Decrypted String: ");
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

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt failed\n");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(IP_ADDR);

    unsigned int n, len;
    uint8_t xorkey;
    unsigned char *decriptedString ;
    ShipData myShip;
    myShip.nextMoveA = 0x00;
    myShip.nextMoveB = 0x00;

    uint16_t frame = 0x00;
    uint16_t input[2] = {myShip.nextMoveA, myShip.nextMoveB};
    uint16_t SEQ;
    uint16_t numberOfObjetcs;

    while(frame != LASTFRAME)
    {
        sendto(sockfd, &inputPackage, sizeof(inputPackage),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));

        n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                    MSG_WAITALL, (struct sockaddr *) &servaddr,
                    &len);

        log_message("Received message with size: %d\n\n", n);

        if (n == -1)
            continue;

        log_message("Server message: ");
        hex_print((unsigned char *)buffer, n);

        xorkey = buffer[0] ^ inputPackage[0];

        decriptedString = (unsigned char *) decryptString(buffer, n, xorkey);

        frame  = decriptedString[0] & 0xFE;
        input[0] = decriptedString[0] & 0x01;
        input[1] = (decriptedString[1] & 0x80) >> 6;
        SEQ = decriptedString[1] & 0x7F;
        numberOfObjetcs  = decriptedString[2];

        if (n != (3 * numberOfObjetcs + 3))
        {
            if (frame == LASTFRAME)
                frame--;
            log_message("Wrong amount of data.\n");
            continue;
        }

        myShip.nextMoveA = 0x00;
        myShip.nextMoveB = 0x00;
        myShip = analyzeData(xorkey, buffer, n, frame, input, SEQ, numberOfObjetcs, decriptedString);

        if(frame == LASTFRAME)
        {
            log_message("\n\n\t********** YOU WON!! **********\n\n");
            break;
        }

        if(myShip.status == SHIP_DESTROYED)
        {
            log_message("\n\n\t********** GAME OVER **********\n\n");
            break;
        }
        else if(myShip.status == ERROR_ON_SHIP_SYSTEM)
            continue;

        log_message("Next Move: 0x%02X 0x%02X\n\n", myShip.nextMoveA, myShip.nextMoveB);
        inputPackage[0] = (((frame >> 1) + 0x01) << 1) + myShip.nextMoveA;
        inputPackage[1] = SEQ + myShip.nextMoveB;
        memset(&buffer, 0, n);
    }
    close(sockfd);
    return 0;
}
