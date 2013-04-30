/* world_adaptor.cc
 *
 * Copyright (c) 2001, 2002 Hansj�rg Malthaner
 * hansjoerg.malthaner@gmx.de
 *
 * This file is part of the Simugraph<->Angband adaption code.
 *
 *
 * This file may be copied and modified freely so long as the above credits,
 * this paragraph, and the below disclaimer of warranty are retained; no
 * financial profit is derived from said modification or copying; and all
 * licensing rights to any modifications are granted to the original author,
 * Hansj�rg Malthaner.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* world_adaptor.c
 *
 * adaption between angband world and simugraph data model
 * Hj. Malthaner, Jan. 2001
 */

#include "hackdef.h"

#include "world_adaptor.h"
#include "world_view.h"
#include "simview.h"
#include "simgraph.h"

#undef MIN
#undef MAX

#include "../angband.h"

#undef bool


/* 
 * Highlit location 
 */
int high_x = -1;
int high_y = -1;


/**
 * Highlite (mark) location x,y
 * @author Hj. Malthaner
 */
void highlite_spot(int x, int y)
{
  high_x = x;
  high_y = y;
}


/**
 * Grid type, default is grid for items and monsters
 * 0: No grid
 * 1: Item/monster grid
 * 2: Full grid
 * @author Hj. Malthaner
 */
static int grid = 1;
                     

/**
 * Show shadow below items and monsters?
 * 0 = no
 * 1 = yes
 * @author Hj. Malthaner
 */
int shadow = 1;


/**
 * Set a grid type (takes argument modulo 3)
 * @author Hj. Malthaner
 */
void set_grid(int no)
{
  grid = no % 3;
}


/**
 * Show which grid type ?
 * @author Hj. Malthaner
 */
int get_grid()
{
  return grid;
}


/**
 * Turn shadows on/off (0=off, 1=on)
 * @author Hj. Malthaner
 */
void set_shadow(int yesno)
{
  shadow = yesno;
}


/**
 * Determines i-offset of the watch point
 * @author Hj. Malthaner
 */
int get_i_off()
{
    const int p_off = (display_get_width() >> 7);
    const int mult = display_get_tile_size() == 32 ? 2 : 1;

#ifdef USE_SMALL_ISO_HACK

    int i_off = 0;
    /* use this for angband 2.9.1 */

    if(p_ptr) {
        /* Angband without view rectangle */
	    // i_off = p_ptr->px;
        i_off = p_ptr->wx + SCREEN_WID / 2 + 1;
    
        i_off -= p_off*mult;    
    }


    /* this is for Pernangband 4.1.5 */
    /*
    i_off = px-p_off*mult;
    */

    return i_off;     
#else

    return 47-p_off;


#endif
}


/**
 * Determines j-offset of the watch point
 * @author Hj. Malthaner
 */
int get_j_off()
{
    const int p_off = display_get_width() >> 7;
    const int mult = display_get_tile_size() == 32 ? 2 : 1;

#ifdef USE_SMALL_ISO_HACK

    int j_off = 0;
    /* use this for angband 2.9.1 */

    if(p_ptr) {
        /* Angband without view rectangle */
	    // j_off = p_ptr->py;
        j_off = p_ptr->wy + SCREEN_HGT / 2 - 1;
        
        j_off -= p_off*mult;
    }


    /* this is for Pernangband 4.1.5 */
    /*
    j_off = py-p_off*mult;
    */

    return j_off;     
#else

    return 10-p_off;

#endif
}



/**
 * Ermittelt x-Offset gescrollter Karte
 * @author Hj. Malthaner
 */
int get_x_off()
{
    return 0;
}


/**
 * Ermittelt y-Offset gescrollter Karte
 * @author Hj. Malthaner
 */
int get_y_off()
{
    return 0;
}

                                     

int init_adaptor()
{
    printf("Preparing display ...\n");
    simgraph_init(672, 480);

    return TRUE;
}


int refresh_display()
{
    display();
    display_flush_buffer();

    return TRUE;
}


int close_adaptor()
{
    simgraph_exit();

    return TRUE;
}
