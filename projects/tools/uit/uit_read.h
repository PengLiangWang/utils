#ifndef _UIT_READER_H
#define _UIT_READER_H

int DispTextAreaFile
    (char *filename,
     int row,int column,int high,int wide,
     char *title,
     char  *foot,
     char *fixline,
     int line_mode);

int ChooseDispTextAreaFile
    (char * fileName,
     int row,int column,int high,int wide,
     char *title,
     char *retString);

#endif
