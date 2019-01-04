#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_material.h"
#include "include/bkp_memory.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/

/***************************************************************************
*	Prototypes 	
**************************************************************************/

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

void bkp_material_initPlatform()
{
	PlatformMaterial[0] = (BKP_PhysProp *) bkp_memory_getTicket(sizeof(BKP_PhysProp));
	PlatformMaterial[0]->sticky = 0.5f; 
	PlatformMaterial[0]->rigidity = 1.0f;
	PlatformMaterial[0]->slide = 0.0f; 
	PlatformMaterial[0]->allowin = BKP_FALSE;
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_material_releasePlatform()
{
	bkp_memory_releaseTicket(PlatformMaterial[0]);
	return;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
