#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

FILE *pLogFile;

#define log_message(F, ...) ({pLogFile = fopen("history.log", "a+"); \
        if (pLogFile == NULL) { \
                fprintf(stderr, "ERROR opening history.log file\n"); \
                exit(1);} \
        fprintf(pLogFile, F, ##__VA_ARGS__); \
        fprintf(stdout, F, ##__VA_ARGS__); \
        fclose(pLogFile);})

#ifdef __cplusplus
}
#endif

#endif // LOG_H_INCLUDED
