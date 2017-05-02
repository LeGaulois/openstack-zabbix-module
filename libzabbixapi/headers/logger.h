#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

typedef FILE *SLOGL_file;
#define SLOGL_MAXFILEPATH             1024


typedef enum { 
    SLOGL_LVL_DEBUG,
    SLOGL_LVL_INFO,
    SLOGL_LVL_NOTICE,
    SLOGL_LVL_WARNING,
    SLOGL_LVL_ERROR,
    SLOGL_LVL_FATAL
} SLOGL_level;



int SLOGL_quit(void);
int SLOGL_init(const char *fileName);
void SLOGL_vprint(SLOGL_level msgLevel, const char *msg, ...);
#endif
