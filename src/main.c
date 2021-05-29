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

/*int nextMove(int numberOfObjetcs, int *types, int *h_positions, int *v_positions, ShipData *ship)*/
int nextMove(int numberOfObjetcs, ObjectsData *objsInFrame, ShipData *ship)
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
                        {
                            printf("\nCASE A\n");
                            goLeft++;

                        }
                    }
                    if (objsInFrame->h_pos_enemyProjec[i] > 0x00 &&
                            objsInFrame->h_pos_enemyProjec[j] == objsInFrame->h_pos_enemyProjec[i] - 1)
                    {
                        if (objsInFrame->v_pos_enemyProjec[j] == 0x01)
                        {
                            printf("\nCASE B\n");
                            goRight++;

                        }
                    }
                    if (ship->x_coordinate == 0x08)
                    {
                        printf("\nCASE C\n");
                        //Move right
                        ship->nextMoveA = 0x00;
                        ship->nextMoveB = 0x80;
                    }
                    if (ship->x_coordinate == 0x00)
                    {
                        printf("\nCASE D\n");
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

                /*if (ship->x_coordinate == 0x08)*/
                /*{*/
                /*    //Move right*/
                /*    ship->nextMoveA = 0x00;*/
                /*    ship->nextMoveB = 0x80;*/
                /*}*/
                /*else if (i != 0 && objsInFrame->v_pos_enemyProjec[i-1] == 0x01)*/
                /*{*/
                /*    //Move right*/
                /*    ship->nextMoveA = 0x00;*/
                /*    ship->nextMoveB = 0x80;*/
                /*}*/
                /*else if (objsInFrame->v_pos_enemyProjec[i+1] == 0x01)*/
                /*{*/
                /*    //Move left*/
                /*    ship->nextMoveA = 0x01;*/
                /*    ship->nextMoveB = 0x00;*/
                /*}*/
                /*else*/
                /*{*/
                /*    //Move left*/
                /*    ship->nextMoveA = 0x01;*/
                /*    ship->nextMoveB = 0x00;*/
                /*}*/
                 return 0;
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
            /*else if (objsInFrame->h_pos_enemys[i] > ship->x_coordinate)*/
            /*{*/
            /*    //Move left*/
            /*    ship->nextMoveA = 0x01;*/
            /*    ship->nextMoveB = 0x00;*/
            /*}*/
            /*else*/
            /*{*/
            /*    //Move right*/
            /*    ship->nextMoveA = 0x00;*/
            /*    ship->nextMoveB = 0x80;*/
            /*}*/
         else
            {
                //Dont move
                ship->nextMoveA = 0x00;
                ship->nextMoveB = 0x00;
            }
    }


    /*for (int i = 0; i < numberOfObjetcs; i++)*/
    /*{*/
    /*    if (types[i] == 0x03)*/
    /*    {*/
    /*        if (h_positions[i] == ship->x_coordinate)*/
    /*        {*/
    /*            if (v_positions[i] <= (ship->y_coordinate + 0x02))*/
    /*            {*/
    /*                if (ship->x_coordinate == 0x08)*/
    /*                {*/
    /*                    //Move right*/
    /*                    ship->nextMoveA = 0x00;*/
    /*                    ship->nextMoveB = 0x80;*/
    /*                }*/
    /*                else*/
    /*                {*/
    /*                    //Move left*/
    /*                    ship->nextMoveA = 0x01;*/
    /*                    ship->nextMoveB = 0x00;*/
    /*                }*/
    /*                break;*/
    /*            }*/
    /*        }*/
    /*        if (h_positions[i] == (ship->x_coordinate + 0x01))*/
    /*        {*/
    /*            if (v_positions[i] <= (ship->y_coordinate + 0x02))*/
    /*            {*/
    /*                if (ship->x_coordinate == 0x07)*/
    /*                {*/
    /*                    //Dont Move*/
    /*                    ship->nextMoveA = 0x00;*/
    /*                    ship->nextMoveB = 0x00;*/
    /*                }*/
    /*                else*/
    /*                {*/
    /*                    //Move left*/
    /*                    ship->nextMoveA = 0x01;*/
    /*                    ship->nextMoveB = 0x00;*/
    /*                }*/
    /*                break;*/
    /*            }*/
    /*        }*/
    /*        if (h_positions[i] == (ship->x_coordinate - 0x01))*/
    /*        {*/
    /*            if (v_positions[i] <= (ship->y_coordinate + 0x02))*/
    /*            {*/
    /*                if (ship->x_coordinate == 0x08)*/
    /*                {*/
    /*                    //Dont Move*/
    /*                    ship->nextMoveA = 0x00;*/
    /*                    ship->nextMoveB = 0x00;*/
    /*                }*/
    /*                else*/
    /*                {*/
    /*                    //Move left*/
    /*                    ship->nextMoveA = 0x01;*/
    /*                    ship->nextMoveB = 0x00;*/
    /*                }*/
    /*                break;*/
    /*            }*/
    /*        }*/
    /*    }*/
    /*    if (types[i] == 0x02)*/
    /*    {*/
    /*        if (h_positions[i] == ship->x_coordinate)*/
    /*        {*/
    /*            //Shoot*/
    /*            ship->nextMoveA = 0x01;*/
    /*            ship->nextMoveB = 0x80;*/
    /*        }*/
    /*        if (h_positions[i] == (ship->x_coordinate + 0x01) ||*/
    /*                h_positions[i] == (ship->x_coordinate - 0x01))*/
    /*        {*/
    /*            //Shoot*/
    /*            ship->nextMoveA = 0x01;*/
    /*            ship->nextMoveB = 0x80;*/
    /*        }*/
    /*        else if (h_positions[i] > ship->x_coordinate)*/
    /*        {*/
    /*            //Move left*/
    /*            ship->nextMoveA = 0x01;*/
    /*            ship->nextMoveB = 0x00;*/
    /*        }*/
    /*        else*/
    /*        {*/
    /*            //Move right*/
    /*            ship->nextMoveA = 0x00;*/
    /*            ship->nextMoveB = 0x80;*/
    /*        }*/
    /*    }*/
    /*}*/
    printf("Next MOVE: 0x%02X\n\n", ship->nextMoveA | ship->nextMoveB);
    return 0;
}

ShipData analyzeData(uint8_t xorkey, char buffer[], unsigned int n,  uint16_t frame, uint16_t input[],
        uint16_t SEQ, uint16_t numberOfObjetcs, unsigned char decriptedString[])
{
    double objectsData = n - 3.0;
    ShipData ship;
    ship.status = 0;

    ObjectsData objsInFrame;
    objsInFrame.enemys = 0;
    objsInFrame.enemysProjectiles = 0;
    objsInFrame.h_pos_enemyProjec = malloc(sizeof(int)*numberOfObjetcs);
    objsInFrame.v_pos_enemyProjec = malloc(sizeof(int)*numberOfObjetcs);
    objsInFrame.h_pos_enemys = malloc(sizeof(int)*numberOfObjetcs);
    objsInFrame.v_pos_enemys = malloc(sizeof(int)*numberOfObjetcs);


    /*int *types = malloc(sizeof(int)*numberOfObjetcs);*/
    /*int *h_positions = malloc(sizeof(int)*numberOfObjetcs);*/
    /*int *v_positions = malloc(sizeof(int)*numberOfObjetcs);*/


    printf("\nCurrent Frame %d\n", frame >> 1);
    printf("Input %d\n", input[0] | input[1]);
    /*printf("SEQ 0x%02X\n", SEQ);*/
    printf("Number Of Objects %d\n\n", numberOfObjetcs);

    if (numberOfObjetcs > 0x00)
    {
        if(objectsData / numberOfObjetcs < 1.0)
        {
            ship.status = -1;
            free(objsInFrame.h_pos_enemyProjec);
            free(objsInFrame.v_pos_enemyProjec);
            free(objsInFrame.h_pos_enemys);
            free(objsInFrame.v_pos_enemys);
            /*free(types);*/
            /*free(h_positions);*/
            /*free(v_positions);*/
            return ship;
        }

        for (int i = 1 ; i <= numberOfObjetcs ; i++)
        {
            printf("\tObject: %d\n", i);
            printf("\t\tType: %d\n", decriptedString[i * 3]);
            printf("\t\tH position: %d\n", decriptedString[(i * 3) + 1]);
            printf("\t\tV position: %d\n", decriptedString[(i * 3) + 2]);

            if (decriptedString[i * 3] == 0x00)
            {
                ship.status = 1;
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
            /**(types + i - 1) = decriptedString[i * 3v];*/
            /**(h_positions + i - 1) = decriptedString[(i * 3) + 1];*/
            /**(v_positions + i - 1) = decriptedString[(i * 3) + 2];*/
        }
        printf("\n\n");

        if (ship.status != 0)
            nextMove(numberOfObjetcs, &objsInFrame, &ship);
            /*nextMove(numberOfObjetcs, types, h_positions, v_positions, &ship);*/
    }
    else
        ship.status = -1;

    free(objsInFrame.h_pos_enemyProjec);
    free(objsInFrame.v_pos_enemyProjec);
    free(objsInFrame.h_pos_enemys);
    free(objsInFrame.v_pos_enemys);
    /*free(types);*/
    /*free(h_positions);*/
    /*free(v_positions);*/
    return ship;
}

/*void readInputFromKeyboard(ShipData *ship)*/
/*{*/
/*    char choice;*/
/*    printf("Enter your move: ");*/
/*    scanf(" %c",&choice);*/
/*    switch (choice) {*/
/*      case 'w':*/
/*      case 'W':*/
/*        //shoot*/
/*        ship->nextMoveA = 0x01;*/
/*        ship->nextMoveB = 0x80;*/
/*        break;*/
/*      case 'a':*/
/*      case 'A':*/
/*         // Move left*/
/*        ship->nextMoveA = 0x01;*/
/*        ship->nextMoveB = 0x00;*/
/*        break;*/
/*      case 's':*/
/*      case 'S':*/
/*         // Dont Move*/
/*        ship->nextMoveA = 0x00;*/
/*        ship->nextMoveB = 0x00;*/
/*        break;*/
/*      case 'd':*/
/*      case 'D':*/
/*        // Move right*/
/*        ship->nextMoveA = 0x00;*/
/*        ship->nextMoveB = 0x80;*/
/*        break;*/
/*      case 'q':*/
/*      case 'Q':*/
/*        printf("\nQuitting...\n");*/
/*        ship->nextMoveA = 0xFF;*/
/*        ship->nextMoveB = 0xFF;*/
/*        break;*/
/*      default:*/
/*        printf("\nWrong input...\n");*/
/*        ship->nextMoveA = 0x00;*/
/*        ship->nextMoveB = 0x00;*/
/*    }*/
/*}*/

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

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
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
    ShipData myShip;
    myShip.nextMoveA = 0x00;
    myShip.nextMoveB = 0x00;

    uint16_t frame = 0x00;
    uint16_t input[2] = {myShip.nextMoveA, myShip.nextMoveB};
    uint16_t SEQ;
    uint16_t numberOfObjetcs;

    printf("\n\t\t\t\t\tBeating Galaga Game\n");
    printf("\n\tControls: s - Don't move/Start; a - Move left; d - Move right; w - Shoot; q - Quit\n");
    printf("\n\t\t\t\t\t\tHave fun =]\n\n");

    /*for(int i = 0; i  < 4 ; i++)*/
    while(frame != 0xFE)
    {
        sendto(sockfd, &inputPackage, sizeof(inputPackage),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));
        /*printf("Message sent to server...\n\n");*/

        n = recvfrom(sockfd, (unsigned char *)buffer, MAXLINE,
                    MSG_WAITALL, (struct sockaddr *) &servaddr,
                    &len);

        printf("\nn : %d\n", n);

        if (n == -1)
            continue;

        /*printf("Received message with size: %d\n", n);*/
        printf("Server message: ");
        hex_print((unsigned char *)buffer, n);

        xorkey = buffer[0] ^ inputPackage[0];

        decriptedString = (unsigned char *) decryptString(buffer, n, xorkey);

        frame  = decriptedString[0] & 0xFE;
        input[0] = decriptedString[0] & 0x01;
        input[1] = (decriptedString[1] & 0x80) >> 6;
        SEQ = decriptedString[1] & 0x7F;
        numberOfObjetcs  = decriptedString[2];

        myShip.nextMoveA = 0x00;
        myShip.nextMoveB = 0x00;
        myShip = analyzeData(xorkey, buffer, n, frame, input, SEQ, numberOfObjetcs, decriptedString);

        if(myShip.status == 0)
        {
            printf("\n\n\t********** GAME OVER **********\n\n");
            break;
        }
        else if(myShip.status == -1)
            continue;

        /*readInputFromKeyboard(&myShip);*/

        /*if ((myShip.nextMoveA & myShip.nextMoveB) == 0xFF)*/
        /*{*/
        /*    printf("\n\n\t********** GAME OVER **********\n\n");*/
        /*    break;*/
        /*}*/

        printf("Next Move: 0x%02X 0x%02X", myShip.nextMoveA, myShip.nextMoveB);
        inputPackage[0] = (((frame >> 1) + 0x01) << 1) + myShip.nextMoveA;
        inputPackage[1] = SEQ + myShip.nextMoveB;
        memset(&buffer, 0, n);

        /*printf("%02x%02x\n", inputPackage[0], inputPackage[1]);*/
    }
    close(sockfd);

    return 0;
}
