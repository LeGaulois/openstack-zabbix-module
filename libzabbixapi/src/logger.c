#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "logger.h"


extern SLOGL_level programLevel;
static SLOGL_file fpSave = NULL;

int SLOGL_quit(void) {
    return fpSave && fclose(fpSave) != EOF;
}

static struct tm *getDate(void) {
    time_t t = time(NULL);
    return localtime(&t);
}

int SLOGL_init(const char *fileName) {
    int ret = 0;

    if (fileName) {
        char fullFileName[SLOGL_MAXFILEPATH];
        struct tm *t = getDate();

        snprintf(fullFileName, sizeof fullFileName, "%s.%d%d%d.log",
            fileName, t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        ret = (fpSave = fopen(fullFileName, "a")) != NULL;
    }

    return ret;
}


static void printLevel(SLOGL_level msgLevel) {
    if (fpSave && msgLevel <= SLOGL_LVL_FATAL) {
        static const char *t[] =
            {"Debug", "Info", "Notice", "Warning", "Error", "Fatal error"};
        fprintf(fpSave, "\n%s - ", t[msgLevel]);
    }
}

static void printDate(void) {
    if (fpSave) {
        struct tm *t = getDate();
        fprintf(fpSave, "%dh%dm%d : ", t->tm_hour, t->tm_min, t->tm_sec);
    }
}

void SLOGL_printHeader(SLOGL_level msgLevel) {
    if (msgLevel >= programLevel) {
        printLevel(msgLevel);
        printDate();
    }
}

void SLOGL_vprint(SLOGL_level msgLevel, const char *msg, ...) {
    if (fpSave && msg && msgLevel >= programLevel) {
        SLOGL_printHeader(msgLevel);
        va_list ap;
        va_start(ap, msg);
        vfprintf(fpSave, msg, ap);
        fflush(fpSave);
    }
}
