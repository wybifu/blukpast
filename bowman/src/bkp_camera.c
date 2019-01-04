#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_camera.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/

typedef int (*pfunc) (BKP_Camera * S, BKP_Dynamics2D P);

/***************************************************************************
*	Prototypes 	
**************************************************************************/

static int scroll2DnoFocus(BKP_Camera * S);
static int scroll2D(BKP_Camera * S);
static void	xscroll2D(BKP_Camera * S, BKP_Dynamics2D P);
static void	yscroll2D(BKP_Camera * S, BKP_Dynamics2D P);
static int find_target(BKP_Camera * S, BKP_Dynamics2D P);

pfunc TBL_Camera[] = {
	camera2DFocus,
	camera2DMoveTo,
	camera2DFixedSetFocus,
	camera2DFixed,
	camera2DLockon,
	camera2DSmoothStd
};

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

int camera2D(BKP_Camera * S, BKP_Dynamics2D P)
{
	if(S->on == BKP_FALSE)
		return BKP_FALSE;

	return TBL_Camera[S->func](S, P);
}

/*--------------------------------------------------------------------------------*/
int camera2DLockon(BKP_Camera * S, BKP_Dynamics2D P)
{
	BKP_Rect dim = S->dim;
	BKP_Rect world = S->world;

	BKP_Vec2 pos;
	pos.x = (P.gbox.x + P.gbox.w / 2) - dim.dimension.w / 2;
	pos.y = (P.gbox.y + P.gbox.h / 2) - dim.dimension.h / 2;

	if(fcmp(pos.x - world.x1 , dim.point.x) < 0 )
		pos.x = world.x1 + dim.point.x;
	else if(fcmp(world.x2 - (pos.x + dim.dimension.w) , dim.point.x ) < 0)
		pos.x = S->pos.x ; 


	if(fcmp(pos.y  - world.y1 , dim.point.y ) < 0)
		pos.y = world.y1 + dim.point.y;
	else if(fcmp(world.y2 - (pos.y  + dim.dimension.h) , dim.point.y ) < 0) 
		pos.y = S->pos.y ; 

	S->pos.x = pos.x; 
	S->pos.y = pos.y; 
	S->delta.x = dim.point.x - pos.x; 
	S->delta.y = dim.point.y - pos.y; 

//	return BKP_FALSE;
	return BKP_TRUE;
}
/*--------------------------------------------------------------------------------*/
int camera2DSmoothStd(BKP_Camera * S, BKP_Dynamics2D P)
{

	xscroll2D(S,P);
	yscroll2D(S,P);

	if(S->ready.x)
	{
		if(S->to.x < S->limitleft)
			S->to.x = S->limitleft;
		else if(S->to.x > S->limitright)
			S->to.x = S->limitright;
	}

	return scroll2D(S);
}

/*--------------------------------------------------------------------------------*/
int camera2DFocus(BKP_Camera * S, BKP_Dynamics2D P)
{
	int ret =  find_target(S, P);
	if(BKP_FALSE == S->ready.x && BKP_FALSE == S->ready.y)
	{
		S->func = S->func_default;
		if(P.dir == BKP_LEFT)
		{
			S->focus.right = S->pos.x + S->screen.w - S->dim.x;
			S->focus.left = S->focus.right - S->focus.length.w;
		}
		else if(P.dir == BKP_RIGHT)
		{
			S->focus.left = S->pos.x + S->dim.x;
			S->focus.right = S->focus.left + S->focus.length.w;
		}

		S->focus.down = S->pos.y + S->screen.h * 3 / 4;
		S->focus.up 	 = S->focus.down - S->focus.length.h;

		S->focus.panicup 	= S->pos.y + S->marginUp ;
		S->focus.panicdown 	= S->pos.y + S->marginDown ;
		S->diff.y = S->limitdown;

	}
	
	return ret;
}
/*--------------------------------------------------------------------------------*/
int camera2DMoveTo(BKP_Camera * S, BKP_Dynamics2D P)
{
	int ret = find_target(S, P);
	if(BKP_FALSE == S->ready.x && BKP_FALSE == S->ready.y)
		return BKP_DONE;

	return ret;
}

/*--------------------------------------------------------------------------------*/
int camera2DFixedSetFocus(BKP_Camera * S, BKP_Dynamics2D P)
{
	camera2DFixed(S, P);
	S->func = S->func_default;
	S->on = BKP_TRUE;
	if(P.dir == BKP_LEFT)
	{
		S->focus.right = S->pos.x + S->screen.w - S->dim.x;
		S->focus.left = S->focus.right - S->focus.length.w;
	}
	else if(P.dir == BKP_RIGHT)
	{
		S->focus.left = S->pos.x + S->dim.x;
		S->focus.right = S->focus.left + S->focus.length.w;
	}

	S->focus.down = S->pos.y + S->screen.h / 2 + S->focus.length.h;
	S->focus.up 	 = S->focus.down - S->focus.length.h;
	return BKP_TRUE;
}

/*--------------------------------------------------------------------------------*/
int camera2DFixed(BKP_Camera * S, BKP_Dynamics2D P)
{
	float px1 = P.gbox.x + P.cpts[BKP_LEFT].pts.x1,
		  py1 = P.gbox.y + P.cpts[BKP_UP].pts.y1;

	S->to.x = px1 - S->screen.w / 2;
	S->to.y = py1 - S->screen.h / 2;

	if(S->to.x < S->limitleft)
		S->to.x = S->limitleft;
	if(S->to.x > S->limitright)
		S->to.x = S->limitright;
	if(S->to.y < S->limitup)
		S->to.y = S->limitup;

	S->pos = S->to;
	S->delta = bkp_vec2(- S->pos.x, - S->pos.y);
	S->on = BKP_FALSE;

	return BKP_TRUE;
}
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
static void	xscroll2D(BKP_Camera * S, BKP_Dynamics2D P)
{
	float px1 = P.gbox.x + P.cpts[BKP_LEFT].pts.x1,
		  px2 = P.gbox.x + P.cpts[BKP_RIGHT].pts.x1;

	if(P.dir != S->dir)
	{
		S->dir = P.dir;
		if(P.dir == BKP_LEFT)
		{
			S->focus.right = S->pos.x + S->screen.w - S->dim.x;
			S->focus.left = S->focus.right - S->focus.length.w;
		}
		if(P.dir == BKP_RIGHT)
		{
			S->focus.left = S->pos.x + S->dim.x;
			S->focus.right = S->focus.left + S->focus.length.w;
		}
	}

	{
		if(P.dir == BKP_LEFT)
		{
			if(px2 < S->focus.left)
			{
				float dist = px1 - S->focus.left;
				S->ready.x = BKP_TRUE;
				S->to.x = S->pos.x + dist;
			}
		}
		else if(P.dir == BKP_RIGHT)
		{
			if(px1 > S->focus.right)
			{
				float dist = px2 - S->focus.right;
				S->ready.x = BKP_TRUE;
				S->to.x = S->pos.x + dist;
			}
		}

		if(S->pos.x <= S->limitleft)
		{
			S->focus.left = S->pos.x + S->dim.x;
			S->focus.right = S->focus.left + S->focus.length.w;
		}
		else if( S->pos.x >= S->limitright)
		{
			S->focus.right = S->pos.x + S->screen.w - S->dim.x;
			S->focus.left = S->focus.right - S->focus.length.w;
		}
	}

	return;
}

/*--------------------------------------------------------------------------------*/
static void	yscroll2D(BKP_Camera * S, BKP_Dynamics2D P)
{
	float py1 = P.gbox.y + P.cpts[BKP_UP].pts.y1,
		  py2 = P.gbox.y + P.cpts[BKP_DOWN].pts.y1;

	if(BKP_MOTION_STAND == P.motion && BKP_FALSE == S->ready.y)
	{
		if(py2 < S->focus.down)
		{
			S->ready.y = BKP_TRUE;
			float dist = py2 - (S->focus.down );
			S->to.y = S->pos.y + dist ;
		}
		else if(py2 > S->focus.up && S->to.y < S->limitdown)
		{
			S->ready.y = BKP_TRUE;
			float dist = py2 - S->focus.down;
			S->to.y = S->pos.y + dist;
		}
		S->diff.y = 0.0f;

	}

	if(py1 < S->focus.panicup)
	{
		S->ready.y = BKP_TRUE;
		S->to.y = S->pos.y - (S->focus.panicup - py1);
	}
	else if(py2 > S->focus.panicdown)
	{
		S->ready.y = BKP_TRUE;
		S->to.y = S->pos.y + (py2 - S->focus.down);
		S->diff.y = P.speed.y;// + S->vslow.y;
	}

	if(S->to.y < S->limitup)
	{
		S->to.y = S->limitup;
		S->focus.panicup 	= S->pos.y + S->marginUp;
		S->focus.panicdown 	= S->pos.y + S->marginDown ;
	}
	else if(S->to.y > S->limitdown)
	{
		S->to.y = S->limitdown;
		S->focus.panicup 	= S->pos.y + S->marginUp ;
		S->focus.panicdown 	= S->pos.y + S->marginDown;
		if(S->limitdown	- S->focus.down > S->screen.h * 3 / 4)
		{
			S->focus.down	+= S->limitdown - (S->screen.h * 3 /4);
			S->focus.up		+= S->focus.down - S->focus.length.h;
		}
	}
}

/*--------------------------------------------------------------------------------*/
static int scroll2D(BKP_Camera * S)
{
	int ret = BKP_FALSE;

	if(S->ready.x)
	{
		float dx = (S->to.x - S->pos.x) / 32;

		ret = BKP_TRUE;
		S->pos.x += dx;

		if(S->pos.x > S->limitleft && S->pos.x < S->limitright)
		{
			S->focus.left += dx;
			S->focus.right += dx;
		}

		if(dx > 0)
		{
			if(S->pos.x >= S->to.x)
			{
				S->pos.x = S->to.x;
				S->ready.x = BKP_FALSE;
			}
		}
		else 
		{
			if(S->pos.x <= S->to.x)
			{
				S->pos.x = S->to.x;
				S->ready.x = BKP_FALSE;
			}
		}

		S->delta.x = -S->pos.x;
	}

	if(BKP_TRUE == S->ready.y)
	{
		float dy;
		ret = BKP_TRUE;

		if(S->diff.y == 0.0f)
		{
			dy	= (S->to.y - S->pos.y) / 16;
			dy += dy > 0 ? .2 : -.2;
		}
		else
		{
			dy = S->diff.y;
		}

		if(S->pos.y > S->limitup && S->pos.y < S->limitdown)
		{
			S->focus.up		+= dy;
			S->focus.down 	+= dy;
		}

		S->focus.panicup 	+= dy;
		S->focus.panicdown	+= dy;


		S->pos.y += dy;

		if(dy > 0)
		{
			if(fcmp(S->pos.y, S->to.y) > 0)
			{
				S->pos.y = S->to.y;
				S->ready.y = BKP_FALSE;
			}
		}
		else
		{
			if(fcmp(S->pos.y, S->to.y) < 0)
			{
				S->pos.y = S->to.y;
				S->ready.y = BKP_FALSE;
			}
		}
		S->delta.y = -S->pos.y;
	}

	return ret;
}

/*--------------------------------------------------------------------------------*/
static int scroll2DnoFocus(BKP_Camera * S)
{
	int ret = BKP_FALSE;

	if(S->ready.x)
	{
		ret = BKP_TRUE;

		float dx = S->diff.x;
		float distancex = abs(S->pos.x - S->to.x);

		if( distancex < S->vslow.x)
		{
			dx = dx * (distancex + .8) / S->vslow.x;
			dx += dx > 0 ? S->minSpeed.x : -S->minSpeed.x ;
		}

		S->pos.x += dx;

		if(dx > 0)
		{
			if(S->pos.x >= S->to.x)
			{
				S->pos.x = S->to.x;
				S->ready.x = BKP_FALSE;
			}
		}
		else 
		{
			if(S->pos.x <= S->to.x)
			{
				S->pos.x = S->to.x;
				S->ready.x = BKP_FALSE;
			}
		}

		S->delta.x = -S->pos.x;
	}

	if(BKP_TRUE == S->ready.y)
	{
		ret = BKP_TRUE;

		float dy = S->diff.y;
		
		float distancey = abs(S->pos.y - S->to.y);
		if( distancey < S->vslow.y)
		{
			dy = dy * (distancey + .8) / S->vslow.y;
			dy += dy > 0 ? S->minSpeed.y : -S->minSpeed.y ;
		}

		S->pos.y += dy;

		if(dy > 0)
		{
			if(S->pos.y >= S->to.y)
			{
				S->pos.y = S->to.y;
				S->ready.y = BKP_FALSE;
			}
		}
		else
		{
			if(S->pos.y <= S->to.y)
			{
				S->pos.y = S->to.y;
				S->ready.y = BKP_FALSE;
			}
		}
		S->delta.y = -S->pos.y;
	}

	return ret;
}

/*--------------------------------------------------------------------------------*/
static int find_target(BKP_Camera * S, BKP_Dynamics2D P)
{
	float px1 = P.gbox.x + P.cpts[BKP_LEFT].pts.x1;
	if(S->pos.x > px1 || (px1 > S->pos.x && (px1 - S->pos.x > S->screen.w)))
	{
		S->to.x = px1 - S->screen.w / 2;
		S->ready.x = BKP_TRUE;

		S->diff.x = (S->pos.x > S->to.x) ? -S->speedInitial.x : S->speedInitial.x;
	}
	float py1 = P.gbox.y + P.cpts[BKP_UP].pts.y1;

	if((S->pos.y < py1 && (S->pos.y - py1 > S->screen.h / 2)) ||  S->pos.y > py1)
	{
		S->ready.y = BKP_TRUE;
		S->to.y = py1 - S->screen.h / 2;

		S->diff.y = (S->pos.y > S->to.y) ? -S->speedInitial.y : S->speedInitial.y;
	}

	return scroll2DnoFocus(S);
}

/*--------------------------------------------------------------------------------*/
