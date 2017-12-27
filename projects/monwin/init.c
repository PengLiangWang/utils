#include "monpub.h"
#include "ttsys.h"
#include "act/DB_PosJnls.h"
#include "datetime.h"

int cps_scrinit()
{
    //int ymax, xmax;
    
    if (initscr() == NULL)
    {
        ELOG(ERROR, "Init screen error !\n");
        return -1;
    }

   
    scrConInit(); 
    keypad(stdscr,TRUE);
    noecho();
    nonl();
    nocbreak();
    
    if (has_colors())
    {
        int iCol;
        start_color();
        for (iCol=0;iCol<64;iCol++) init_pair(iCol,iCol&7,iCol>>3);
    }
    
    clear();
    refresh();
    
    return 0;
}

