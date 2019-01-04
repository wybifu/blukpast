#include <stdlib.h>
#include <stdio.h>

/*copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/graphics.h"

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

static void	stc_drawPlatform2DBox(void);
static void stc_draw_collider2DLines(void);
static void stc_draw_scroller(void);


/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

BKP_Rotate r;
void Ugp_draw()
{
	
	//bkp_graphics_ClearColori(0x99, 0x99, 0xF1, 0xFF);
	bkp_graphics_ClearColori(0x8F, 0x8F, 0x8F, 0xFF);
	bkp_graphics_2DReady(BKP_TRUE);

	BKP_Rect dest,src;
	bkp_graphics_2DsetColori(0x0,0xff,0x10,255);


#ifdef BBOXES
	stc_draw_collider2DLines();
#endif

#ifdef BBOXES
	stc_drawPlatform2DBox();
#endif

	src.point = bkp_numcal_vec2(G[0]->spriteInfo.x, G[0]->spriteInfo.y);
	src.dimension = bkp_numcal_vec2(G[0]->spriteInfo.w, G[0]->spriteInfo.h);

	dest.point = bkp_numcal_vec2(G[0]->dyn.gbox.x, G[0]->dyn.gbox.y);
	dest.dimension = bkp_numcal_vec2(G[0]->dyn.gbox.w, G[0]->dyn.gbox.h);
	
	//bkp_graphics_drawSurface(G[0]->spritesheet, &dest, &src, NULL, BKP_GRAPHICS_FLIPNONE, NULL);
	bkp_graphics_drawSurface(G[0]->spritesheet, &dest, &src, NULL, G[0]->flip, NULL);

#ifdef PLAYER_BBOXES

	bkp_graphics_2DsetColori(0x0,0xff,0xff,0xc0);
	dest.dimension = bkp_numcal_vec2(3, 3);
	for(int i = 0; i < 4; ++i)
	{
		float x = G[0]->dyn.gbox.x + G[0]->dyn.cpts[i].pts.x1,
			  y = G[0]->dyn.gbox.y + G[0]->dyn.cpts[i].pts.y1,
			  w = 2,h = 2;
		if(i == BKP_UP || i == BKP_DOWN)
			w = G[0]->dyn.cpts[i].pts.x2 - G[0]->dyn.cpts[i].pts.x1 ;
		else
			h = G[0]->dyn.cpts[i].pts.y2 - G[0]->dyn.cpts[i].pts.y1 ;
	
		dest.point  = bkp_numcal_vec2( x, y);
		dest.dimension = bkp_numcal_vec2( w ,h);
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);
	}

	bkp_graphics_2DsetColori(0xff,0x0,0x0,0xc0);
	float x = G[0]->dyn.gbox.x + G[0]->dyn.cpts[BKP_RIGHT].pts.x1,
		  y = G[0]->dyn.gbox.y + G[0]->dyn.climbpts.x;
	dest.point  = bkp_numcal_vec2( x, y);
	dest.dimension = bkp_numcal_vec2(4, 4);
	bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);

	y = G[0]->dyn.gbox.y + G[0]->dyn.climbpts.y;
	dest.point  = bkp_numcal_vec2( x, y);
	bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);

	x = G[0]->dyn.gbox.x + G[0]->dyn.cpts[BKP_LEFT].pts.x1,
	y = G[0]->dyn.gbox.y + G[0]->dyn.climbpts.x;
	dest.point  = bkp_numcal_vec2( x, y);
	bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);
	y = G[0]->dyn.gbox.y + G[0]->dyn.climbpts.y;
	dest.point  = bkp_numcal_vec2( x, y);
	bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);

	dest.point = bkp_vec2(
			bkp_numcal_min2f(G[0]->dyn.gbox.x + G[0]->dyn.cpts[BKP_LEFT].pts.pt1.x , G[0]->dyn.gbox.x + G[0]->dyn.cpts[BKP_LEFT].pts.pt1.x+ G[0]->dyn.speed.x),
			bkp_numcal_min2f(G[0]->dyn.gbox.y + G[0]->dyn.cpts[BKP_UP].pts.pt1.y , G[0]->dyn.gbox.y + G[0]->dyn.cpts[BKP_UP].pts.pt1.y+ G[0]->dyn.speed.y));
	dest.dimension = bkp_vec2( 
			fabs(G[0]->dyn.cpts[BKP_LEFT].pts.pt1.x - G[0]->dyn.cpts[BKP_RIGHT].pts.pt1.x) + G[0]->dyn.speed.x,
			fabs(G[0]->dyn.cpts[BKP_DOWN].pts.pt1.y - G[0]->dyn.cpts[BKP_UP].pts.pt1.y)+ G[0]->dyn.speed.y);
	bkp_graphics_2DsetColori(0x0,0xFF,0x0,0xFF);
	//bkp_graphics_2DsetColori(G[0]->spritesheet->colors.r, G[0]->spritesheet->colors.g, G[0]->spritesheet->colors.b, G[0]->spritesheet->colors.a);
	bkp_graphics_drawRectangle(&dest,NULL, BKP_FALSE);
	dest.point = G[0]->dyn.gbox.point;
	dest.dimension = G[0]->dyn.gbox.dimension;
	bkp_graphics_2DsetColori(0x0,0x0,0xFF,0xFF);
	bkp_graphics_drawRectangle(&dest,NULL, BKP_FALSE);

	//fprintf(stderr,"P (%.2f, %.2f)\n",dest.point.x,dest.point.y); 
#endif

	stc_draw_scroller();

	bkp_graphics_OnScreentext();

	bkp_graphics_2DFlush();
	return;
}

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
void draw_axis( void )
{
	BKP_Rect dest;

	BKP_Vec2i v = bkp_graphics_getWindowDimensions();

	bkp_graphics_2DsetColori(0x0,0x0,0x10,255);
	dest = bkp_rect(v.x/ 2, 0, 2, v.y);
	bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);
	dest = bkp_rect(0, v.y/ 2, v.x, 2);
	bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);

	return;

}
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
static void	stc_drawPlatform2DBox(void)
{
	if(Platforms != NULL)
	{
		BKP_Rect dest;
		BKP_Platform2D * map = NULL;
		bkp_platform_2DMap_lrewind(Platforms);
		while(map = bkp_platform_2DMap_lget(Platforms), NULL != map)
		{
			bkp_graphics_setColorvi(map->color);
			dest.point = map->dyn.pbox.point;
			dest.dimension = map->dyn.pbox.dimension;
			BKP_Rotate * pRot = NULL;

			if(map->isRotated == BKP_TRUE)
				pRot = &map->rotate;

			bkp_graphics_drawRectangle(&dest, pRot, map->visible_fill);
		}
	}
}
/*--------------------------------------------------------------------------------*/
static void stc_draw_collider2DLines(void)
{
	if(Map != NULL)
	{
		BKP_Rect dest;
		BKP_CollisionLine2D * map = NULL;
		bkp_collider_2DMap_lrewind(Map);
		while(map = bkp_collider_2DMap_lget(Map), NULL != map)
		{
			dest.point = map->pts.pt1;
			dest.dimension = map->dim;
			BKP_Rotate * pRot = NULL;

			if(BKP_TRUE == map->contact)
				bkp_graphics_2DsetColori(0xff, 0x0, 0x0, 0xff);
			else
				bkp_graphics_2DsetColori(0xff, 0xff, 0xff, 0xff);

			if(map->isRotated == BKP_TRUE)
				pRot = &map->rotate;
			
			bkp_graphics_drawRectangle(&dest, pRot, BKP_FALSE);

			
			bkp_graphics_2DsetColori(0xff, 0x0, 0x0, 0xff);
			dest.point = map->bbox.point;
			dest.dimension = map->bbox.dimension;
			bkp_graphics_drawRectangle(&dest, NULL, BKP_FALSE);

			bkp_graphics_2DsetColori(0x0, 0x0, 0xff, 0xff);
			dest.point = map->pts.pt1;
			dest.dimension = bkp_vec2(2,2);
			bkp_graphics_drawRectangle(&dest, NULL, BKP_FALSE);
			dest.point = map->pts.pt2;
			bkp_graphics_drawRectangle(&dest, NULL, BKP_FALSE);
		}
	}
}
/*--------------------------------------------------------------------------------*/
static void stc_draw_scroller(void)
{
	//return;
	BKP_Rect dest, world, focus; 
	BKP_Vec2 pos;
	int func = bkp_graphics_getCameraFunc();

	if(func == BKP_CAMERAFUNC_LOCKON)
	{
		bkp_graphics_2DsetColori(0xff,0xff,0xff,0x1f);
		BKP_Rect d = bkp_graphics_2DgetCameraDim();

		dest.point = bkp_graphics_2DgetCameraPos();// + stc_2D->scroller.dim.point.x ;
		dest.dimension = d.dimension;
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);
	}
	else
	{
		world			= bkp_graphics_2DgetCameraWorld();
		focus			= bkp_graphics_2DgetCamerafocus();
		pos				= bkp_graphics_2DgetCameraPos();
		BKP_Vec2 panic	= bkp_graphics_2DgetCamerapanic();
		BKP_Vec2i scr	= bkp_graphics_getWindowDimensions();

		bkp_graphics_2DsetColori(0xff,0xff,0x0,0x5f);
		dest.dimension = bkp_vec2(world.x2 - world.x1, 1.0f);
		dest.point = bkp_vec2(world.x1, focus.y1);
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);
		dest.point = bkp_vec2(world.x1, focus.y2);
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);

		dest.point.x = world.x1;
		bkp_graphics_2DsetColori(0xff,0x8f,0x0,0xff);
		dest.point.y = panic.v[0];
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);
		bkp_graphics_2DsetColori(0x0,0x0,0x0,0xff);
		dest.point.y = panic.v[1];
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);


		bkp_graphics_2DsetColori(0xff,0xff,0x0,0x5f);
		dest.dimension = bkp_vec2(1.0f,world.y2 - world.y1);
		dest.point = bkp_vec2(focus.x1, world.y1);
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);
		dest.point = bkp_vec2(focus.x2, world.y1);
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);



		bkp_graphics_2DsetColori(0xff,0x0,0x0,0xff);
		dest.point.x = pos.x + scr.w / 2;
		dest.point.y = world.y1;
		bkp_graphics_drawRectangle(&dest,NULL, BKP_TRUE);
	}

	return;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

