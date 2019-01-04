#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/macro.h"
#include "include/bkp_collider.h"
#include "include/bkp_logger.h"
#include "include/bkp_physics.h"
#include "include/bkp_geometry.h"
#include "include/bkp_helper.h"
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

static int stc_collision_iterations = 100;
static int stc_ids = 0;

/***************************************************************************
*	Prototypes 	
**************************************************************************/

static void bkp_collider_2DinitLine(BKP_CollisionLine2D *cl, BKP_Rect pts, BKP_Vec3 rinfo, void * phyprop);

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

BKP_ColliderMap * bkp_collider_2DcreateMap()
{
	BKP_ColliderMap * cm = (BKP_ColliderMap *) bkp_memory_getTicket(sizeof(BKP_ColliderMap));
	cm->left= cm->right = NULL;
	cm->total = 0;
	cm->current = NULL;

	return cm;
}

/*--------------------------------------------------------------------------------*/
void bkp_collider_2DdestroyMap(BKP_ColliderMap * Map)
{
	BKP_CollisionLine2D * p = NULL;

	bkp_collider_2DMap_lrewind(Map);

	while(p = bkp_collider_2DMap_lpop(Map), NULL != p)
		bkp_memory_releaseTicket(p);

	bkp_memory_releaseTicket(Map);
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_collider_2DMap_remove(BKP_ColliderMap * map, BKP_CollisionLine2D * cl)
{
	struct local_membrane_l * mem = (struct local_membrane_l *) cl->membrane;
	if(NULL != mem->selfleft)
		mem->selfleft->selfright = mem->selfright;
	else
		map->left = mem->selfright;

	if(NULL != mem->selfright)
		mem->selfright->selfleft = mem->selfleft;
	else
		map->right = mem->selfleft;

	bkp_memory_releaseTicket(cl);
	bkp_memory_releaseTicket(mem);

	return;
}


/*--------------------------------------------------------------------------------*/
BKP_CollisionLine2D * bkp_collider_2DMap_lpush(BKP_ColliderMap * cm, BKP_Rect pts, BKP_Vec3 rinfo, void * phyprop)
{

	struct local_membrane_l * lc = (struct local_membrane_l *) bkp_memory_getTicket(sizeof(struct local_membrane_l));
	lc->collision_line = (BKP_CollisionLine2D *) bkp_memory_getTicket(sizeof(BKP_CollisionLine2D));
	bkp_collider_2DinitLine(lc->collision_line, pts, rinfo, phyprop);

	lc->collision_line->membrane = (void *) lc;
	lc->selfleft = NULL;
	lc->selfright = cm->left;

	if(NULL != cm->left)
		cm->left->selfleft = lc;
	else
		cm->right = cm->current = lc;

	cm->left = lc;

	cm->total += 1;

	return lc->collision_line;
}

/*--------------------------------------------------------------------------------*/
BKP_CollisionLine2D * bkp_collider_2DMap_lpop(BKP_ColliderMap * map)
{
	if(NULL == map->left)
		return NULL;

	map->total -= 1;
	struct local_membrane_l *lc = map->left;
	map->left = lc->selfright;
	if(NULL != lc->selfright)
		lc->selfright->selfleft = NULL;

	BKP_CollisionLine2D * line = lc->collision_line;
	bkp_memory_releaseTicket(lc);

	return line;
}

/*--------------------------------------------------------------------------------*/
BKP_CollisionLine2D * bkp_collider_2DMap_rpush(BKP_ColliderMap * cm, BKP_Rect pts, BKP_Vec3 rinfo, void * phyprop)
{

	struct local_membrane_l * lc = (struct local_membrane_l *) bkp_memory_getTicket(sizeof(struct local_membrane_l));
	lc->collision_line = (BKP_CollisionLine2D *) bkp_memory_getTicket(sizeof(BKP_CollisionLine2D));
	bkp_collider_2DinitLine(lc->collision_line, pts, rinfo, phyprop);

	lc->collision_line->membrane = (void *) lc;
	lc->selfright = NULL;
	lc->selfleft= cm->right;

	if(NULL != cm->right)
		cm->right->selfright = lc;
	else
		cm->left = cm->current = lc;

	cm->right = lc;
	cm->total += 1;

	return lc->collision_line;
}

/*--------------------------------------------------------------------------------*/
BKP_CollisionLine2D * bkp_collider_2DMap_rpop(BKP_ColliderMap * map)
{
	if(NULL == map->right)
		return NULL;

	map->total -= 1;
	struct local_membrane_l *lc = map->right;
	map->left = lc->selfleft;
	if(NULL != lc->selfleft)
		lc->selfright->selfright = NULL;

	BKP_CollisionLine2D * line = lc->collision_line;
	bkp_memory_releaseTicket(lc);
	return line;
}

/*--------------------------------------------------------------------------------*/
void bkp_collider_2DMap_lrewind(BKP_ColliderMap * map)
{
	map->current = map->left;
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_collider_2DMap_rrewind(BKP_ColliderMap * map)
{
	map->current = map->right;
	return;
}

/*--------------------------------------------------------------------------------*/
BKP_CollisionLine2D * bkp_collider_2DMap_lget(BKP_ColliderMap * map)
{
	if(NULL == map->current)
			return NULL;

	BKP_CollisionLine2D * lc = map->current->collision_line;
	map->current = map->current->selfright;
	return lc;
}

/*--------------------------------------------------------------------------------*/
BKP_CollisionLine2D * bkp_collider_2DMap_rget(BKP_ColliderMap * map)
{
	if(NULL == map->current)
			return NULL;

	BKP_CollisionLine2D * lc = map->current->collision_line;
	map->current = map->current->selfleft;
	return lc;
}

/*--------------------------------------------------------------------------------*/
static void bkp_collider_2DinitLine(BKP_CollisionLine2D *cl, BKP_Rect pts, BKP_Vec3 rinfo, void * phyprop)
{
	
	cl->pts = pts;
	cl->dim = bkp_vec2(fabs(pts.x2 - pts.x1), fabs(pts.y2 - pts.y1)); // maybe need dim after rotation, let see later
	cl->nreference = 0;;
	cl->phys = phyprop;

	//forcer tout les angles dans l'interval [0;360[
	if(fabs(rinfo.z) >= 360)
	{
		int an 	= rinfo.z / 360;
		int mod	= (int)rinfo.z % 360;
		rinfo.z	= mod + (rinfo.z - (an * 360 + mod));
	}

	float angle = fabs(rinfo.z);
	if(angle < 45.0f || angle > 315.0f || (angle  > 135.0f && angle < 225.0f))
		cl->allow_runup = BKP_TRUE;
	else
		cl->allow_runup = BKP_FALSE;

	angle = BKP_NUMCAL_DEG2RAD(rinfo.z);
	cl->rotate.center = bkp_vec3(rinfo.x, rinfo.y, 0.0f);
	cl->rotate.angle_deg = rinfo.z;
	cl->rotate.angle = angle;
	cl->rotate.axis = bkp_vec3(0.0f, 0.0f, 1.0f);
	cl->rotate.sin_a = sin(angle);
	cl->rotate.cos_a = cos(angle);
	cl->contact = BKP_FALSE;
	cl->id = ++stc_ids;

	if(0.0f == cl->rotate.angle_deg)
		cl->isRotated = BKP_FALSE;
	else
		cl->isRotated = BKP_TRUE;

	int h = 1;   //level editor can onli make 2 type of collider, vertical and horizontal, 
				//anything else should be a rotation of those 2
	if(pts.pt1.x == pts.pt2.x) h = 0;

	pts.pt1 = bkp_geometry_rotatePoint2D(pts.pt1, cl->rotate, BKP_ROTATE_DEFAULT);
	pts.pt2 = bkp_geometry_rotatePoint2D(pts.pt2, cl->rotate, BKP_ROTATE_DEFAULT);
	if(fabs(rinfo.z) == 90.0f ||fabs(rinfo.z) == 270.0f)
		pts.pt2.x = pts.pt1.x;
	else if((fabs(rinfo.z) == 0.0f ||fabs(rinfo.z) == 180.0f) && h == 1)
		pts.pt2.y = pts.pt1.y;
	cl->line = bkp_geometry_get2Dline(pts.pt1, pts.pt2);

	/*
	 * il arrive qu'apres rotation de pi/2, on ait  des abcisses differentes 
	 * d'une valeur 0.000000000001 pres ou moins. donc je force l'egalite en supposant
	 * que pour 1/10000eme de pixel on ne voit pas la difference.
	 */
	if((int)cl->line.pt1.x * 10000 == (int) cl->line.pt2.x * 10000)
	{
		cl->line.isXaxis = BKP_TRUE;
		cl->allow_runup = BKP_FALSE;
	}


	cl->bbox.point = bkp_vec2(bkp_numcal_minvec2(bkp_vec2(cl->line.pt1.x, cl->line.pt2.x)), 
									 bkp_numcal_minvec2(bkp_vec2(cl->line.pt1.y, cl->line.pt2.y))); 

	cl->bbox.dimension = bkp_vec2(fabs(cl->line.pt1.x - cl->line.pt2.x), fabs(cl->line.pt1.y - cl->line.pt2.y)); 

	return;
}

/*--------------------------------------------------------------------------------*/
BKP_Rect bkp_collider_getWorldBound2D(BKP_ColliderMap * map)
{
	BKP_CollisionLine2D * p = NULL;
	float x1 , y1 ,  x2 , y2;
	x1 = x2 = y1 = y2 = 0.0f;

	bkp_collider_2DMap_lrewind(map);
	while(p = bkp_collider_2DMap_lget(map), NULL != p)
	{

		if(p->line.pt1.x <= x1)
			x1 = p->line.pt1.x;
		if(p->line.pt2.x <= x1)
			x1 = p->line.pt2.x;

		if(p->line.pt1.x >= x2)
			x2 = p->line.pt1.x;
		if(p->line.pt2.x >= x2)
			x2 = p->line.pt2.x;

		if(p->line.pt1.y <= y1)
			y1 = p->line.pt1.y;
		if(p->line.pt2.y <= y1)
			y1 = p->line.pt2.y;

		if(p->line.pt1.y >= y2)
			y2 = p->line.pt1.y;
		if(p->line.pt2.y >= y2)
			y2 = p->line.pt2.y;

	}
	return bkp_rect(x1, y1, x2, y2);
}

/*--------------------------------------------------------------------------------*/
int bkp_collider_2DgetColliderProximity(BKP_ColliderMap * Map,BKP_CollisionLine2D ** res,BKP_Rect rect)
{
	int icl = 0;
	BKP_CollisionLine2D * p = NULL;
	bkp_collider_2DMap_lrewind(Map);

	while(p = bkp_collider_2DMap_lget(Map), NULL != p)
	{
		
		if(BKP_FALSE == bkp_geometry_rectIntersectDm(rect, p->bbox)) continue;

		res[icl] = p;
		++icl;
	}

	return icl;
}
/*--------------------------------------------------------------------------------*/
BKP_Collision2DReturn bkp_collider_2Dcollision(BKP_Dynamics2D * a, BKP_ColliderMap * Map, BKP_Vec2 mov)
{
	BKP_CollisionLine2D * CL[Map->total];
	int ncl, 
		collides = BKP_FALSE,
		c_dir[BKP_2D_MAX_DIR] = {BKP_FALSE};
	float newx = mov.x,
		  newy = mov.y;

	BKP_Rect player_bound = bkp_rect(
			bkp_numcal_min2f(a->gbox.x, a->gbox.x + newx),
			bkp_numcal_min2f(a->gbox.y, a->gbox.y + newy),
			bkp_numcal_max2f(a->gbox.x + a->pbox.w, a->gbox.x + a->pbox.w + newx),
			bkp_numcal_max2f(a->gbox.y + a->pbox.h, a->gbox.y + a->pbox.h + newy));

	BKP_Collision2DReturn result = bkp_createCollision2DReturn(mov);

	if(newx> 0) a->cpts[BKP_LEFT].l = NULL;
	else if(newx< 0) a->cpts[BKP_RIGHT].l = NULL;
	if(newy> 0) a->cpts[BKP_UP].l = NULL;

	ncl = bkp_collider_2DgetColliderProximity(Map, CL, player_bound);
	if(0 == ncl)
	{
		for(int i = 0; i < BKP_2D_MAX_DIR; ++i)
			a->cpts[i].l = NULL;

		return result;
	}

//	fprintf(stderr,"proximity : %d\n", ncl);

	BKP_Rect colpts[BKP_2D_MAX_DIR];
	colpts[BKP_UP] 	  = bkp_rect(a->gbox.x + a->cpts[BKP_UP].pts.pt1.x, a->gbox.y + a->cpts[BKP_UP].pts.pt1.y,
			a->gbox.x + a->cpts[BKP_UP].pts.pt2.x, a->gbox.y + a->cpts[BKP_UP].pts.pt2.y);
	colpts[BKP_LEFT]  = bkp_rect(a->gbox.x + a->cpts[BKP_LEFT].pts.pt1.x, a->gbox.y + a->cpts[BKP_LEFT].pts.pt1.y,
			a->gbox.x + a->cpts[BKP_LEFT].pts.pt2.x, a->gbox.y + a->cpts[BKP_LEFT].pts.pt2.y);
	colpts[BKP_DOWN]  = bkp_rect(a->gbox.x + a->cpts[BKP_DOWN].pts.pt1.x, a->gbox.y + a->cpts[BKP_DOWN].pts.pt1.y,
			a->gbox.x + a->cpts[BKP_DOWN].pts.pt2.x, a->gbox.y + a->cpts[BKP_DOWN].pts.pt2.y);
	colpts[BKP_RIGHT] = bkp_rect(a->gbox.x + a->cpts[BKP_RIGHT].pts.pt1.x, a->gbox.y + a->cpts[BKP_RIGHT].pts.pt1.y,
			a->gbox.x + a->cpts[BKP_RIGHT].pts.pt2.x, a->gbox.y + a->cpts[BKP_RIGHT].pts.pt2.y);


	a->motion = BKP_MOTION_FALL;
		
	int contact = BKP_TRUE;
	for(int iteration = 0; iteration < stc_collision_iterations && contact; ++iteration)
	{
		contact = BKP_FALSE;
		for(int o = 0; o < ncl; ++o)
		{
			float vectorLength = sqrtf(newx * newx + newy * newy);

			if(vectorLength == 0) continue; //avoid tmpx or tmpy = NaN

			int hasFloating = fcmp(vectorLength, (float) (int) vectorLength) > 0 ? BKP_TRUE : BKP_FALSE;
			int segments = (int) vectorLength + (BKP_TRUE == hasFloating ? 1 : 0);
			float tmpx = newx / vectorLength,
				  tmpy = newy / vectorLength;

			for(int iseg = 1; iseg <= segments && BKP_FALSE == contact; ++iseg)
			{
//				fprintf(stderr,"\tsegment %d/%d\n",iseg, segments);
				for(int dir = 0; dir < BKP_2D_MAX_DIR; ++dir)
				{
					if(dir == BKP_UP    && newy >= 0) continue;
					if(dir == BKP_LEFT  && newx >= 0) continue;
					if(dir == BKP_DOWN  && newy <= 0) continue;
					if(dir == BKP_RIGHT && newx <= 0) continue;

//	fprintf(stderr,"\t\t(%p)%d : allow : %d, %s(%p) --- ",CL[o],CL[o]->id,CL[o]->allow_runup,g_bkp_strdir[dir],a->cpts[dir].l);
					if( CL[o] == a->cpts[dir].l && BKP_TRUE == CL[o]->allow_runup)
					{
						if(dir == BKP_DOWN || (dir != BKP_UP && CL[o] == a->cpts[BKP_DOWN].l) )
						{
							fprintf(stderr,"Ignored\n");
							continue;
						}
					}

					BKP_Rect mvpts = bkp_rect(colpts[dir].pt1.x + iseg * tmpx, colpts[dir].pt1.y + iseg * tmpy,
							colpts[dir].pt2.x + iseg * tmpx, colpts[dir].pt2.y + iseg * tmpy);

					a->cpts[dir].l = NULL;
					fprintf(stderr,"checking : ");
					if(BKP_TRUE == bkp_collider_motionSurfaceIntersect(colpts[dir], mvpts, CL[o]))
					{
						fprintf(stderr,"Detected\n");
						a->cpts[dir].l = CL[o];
//						fprintf(stderr,"avec %d\n",CL[o]->id); /*usleep(30000000)*/;exit(1);
						contact = BKP_TRUE;
						collides = BKP_TRUE;
						c_dir[dir] = BKP_TRUE;

						if(dir == BKP_UP ||dir == BKP_DOWN)
						{
							a->speed.y = 0.0f;
							newy = (iseg -1) * tmpy;
						}
						else
						{
							newx = (iseg -1) * tmpx;
							if(BKP_FALSE == CL[o]->allow_runup)
								a->speed.x = 0.0f;
						}
					}
					else
						fprintf(stderr,"Passed\n");
				}
			}
		}
	}

	if(BKP_TRUE == collides)
	{
		result.mov 				= bkp_vec2(newx, newy);
	//	result.inside 			= bkp_platform_inPlatform2D(a, mbounds, nbounds, newx, newy);
		result.collides			= collides;
		for(int i = 0 ; i< BKP_2D_MAX_DIR; ++i)
			result.dirs[i] = c_dir[i];
	}


	return result;  //A verifier
}

/*--------------------------------------------------------------------------------*/
int bkp_collider_motionSurfaceIntersect(BKP_Rect origin, BKP_Rect pos, BKP_CollisionLine2D * cl)
{
	BKP_2Dline l1, l2;

	l1 = bkp_geometry_get2Dline(origin.pt1, pos.pt1);
	l2 = bkp_geometry_get2Dline(origin.pt2, pos.pt2);
	
	BKP_Vec2 pt1 = bkp_vec2(pos.pt1.x, pos.pt1.y), pt2 = bkp_vec2(pos.pt2.x, pos.pt2.y);
	int val1 = bkp_geometry_getSegmentsIntersection2(l1, cl->line, &pt1),
		val2 = bkp_geometry_getSegmentsIntersection2(l2, cl->line, &pt2);

	if( 0 != val1 || 0 != val2)
		return BKP_TRUE;
	

	l1 = bkp_geometry_get2Dline(pos.pt1, pos.pt2);

	if( BKP_TRUE == l1.isXaxis && BKP_TRUE == cl->line.isXaxis && fabs(l1.pt1.x - cl->line.pt1.x) < 1)
		return BKP_TRUE;
	if(0 == l1.eq.a_ && 0 == cl->line.eq.a_ && fabs(l1.pt1.y - cl->line.pt1.y) < 1)
		return BKP_TRUE;

	val1 = bkp_geometry_getSegmentsIntersection2(l1, cl->line, &pt1);

	if(val1 == 1)
		return BKP_TRUE;

	return BKP_FALSE;
}

/*--------------------------------------------------------------------------------*/
int bkp_collider_horizontalMove(BKP_Dynamics2D *dyn, int dir)
{
	if(NULL == dyn->cpts[BKP_DOWN].l)
		return BKP_MOTION_FALL;

	float dx, px, py;
	float angle;
	BKP_CollisionLine2D * cl = dyn->cpts[dir].l;
	fprintf(stderr,"horiz : cl = %p\n",cl);
	if(dir == BKP_RIGHT)
	{
		dx = dyn->force.x;
		if(dyn->cpts[BKP_RIGHT].l != dyn->cpts[BKP_DOWN].l)
		{
			py = dyn->gbox.y + dyn->cpts[BKP_RIGHT].pts.pt2.y;
			px = dyn->gbox.x + dyn->cpts[BKP_RIGHT].pts.pt2.x;
		}
	}
	else
	{
		dx = -dyn->force.x;
		if(dyn->cpts[BKP_LEFT].l != dyn->cpts[BKP_DOWN].l)
		{
			py = dyn->gbox.y + dyn->cpts[BKP_LEFT].pts.pt2.y;
			px = dyn->gbox.x + dyn->cpts[BKP_LEFT].pts.pt2.x;
		}
	}

	//fprintf(stderr,"cl = %p\n",cl);
	if(NULL != cl && cl != dyn->cpts[BKP_DOWN].l)
	{
		if(BKP_TRUE == cl->allow_runup)
		{
			float cptDy =  cl->line.eq.a_ * px  + cl->line.eq.b_; // lower right point
			if(fcmp(py , cptDy) == -1)
			{
				dyn->cpts[BKP_DOWN].l = cl;
				fprintf(stderr,"should go on right platform\n");
			}
			else
			{
				/*
				   float miny = bkp_numcal_min2f(dyn->cpts[BKP_RIGHT].pt1.y,dyn->cpts[BKP_RIGHT].pt2.y);
				   if(dyn->cpts[BKP_RIGHT].pt2.y <= miny)
				   {
				   }
				   */
			}
		}
		else
		{
		}

	}

	dyn->speed.x += dx;
	cl = dyn->cpts[BKP_DOWN].l;
	angle = cl->rotate.angle_deg;
	if(angle != 0.0f && angle != 180.0f)
	{
		px = dyn->gbox.x + dyn->cpts[BKP_DOWN].pts.pt1.x;
		py = dyn->gbox.y + dyn->cpts[BKP_DOWN].pts.pt1.y;

		BKP_2Dline new_line = bkp_geometry_get2DlineFromColinear(bkp_vec2(px, py), cl->line.colinear);
		bkp_geometry_printLine("new line", new_line);
		bkp_geometry_printLine("colinear", cl->line);

		float y = new_line.eq.a_ * (px + dyn->speed.x) + new_line.eq.b_;
		dyn->speed.y = y - py;
		fprintf(stderr,"down collide with %p right %p, left %p\n", cl, dyn->cpts[BKP_RIGHT].l, dyn->cpts[BKP_LEFT].l);
	}

	fprintf(stderr,"*************************************\n");

	return BKP_MOTION_STAND;
}

/*--------------------------------------------------------------------------------*/
void bkp_collider_agenton(BKP_Dynamics2D *a)
{
	a->motion = BKP_MOTION_FALL;
	if(NULL == a->cpts[BKP_DOWN].l)
		return;

	BKP_CollisionLine2D * cl = a->cpts[BKP_DOWN].l;
	float x = a->gbox.x + a->cpts[BKP_DOWN].pts.pt1.x;
	float y = a->gbox.y + a->cpts[BKP_DOWN].pts.pt1.y;
	float x2 = a->gbox.x + a->cpts[BKP_DOWN].pts.pt2.x;
	float y2 = a->gbox.y + a->cpts[BKP_DOWN].pts.pt2.y;
	int stop_it = 0;
	BKP_Vec2 pt;

	BKP_2Dline l1 = bkp_geometry_get2Dline(bkp_vec2(x,y),bkp_vec2(x,y + a->speed.y));
	if( 1 == bkp_geometry_getSegmentsIntersection2(l1, cl->line,&pt))
	{
		stop_it = 1;
	}
	else
	{
		l1 = bkp_geometry_get2Dline(bkp_vec2(x2,y2),bkp_vec2(x2,y2 + a->speed.y));
		if(1 == bkp_geometry_getSegmentsIntersection2(l1, cl->line,&pt))
		{
			stop_it = 1;
		}
		else
		{
			l1 = bkp_geometry_get2Dline(bkp_vec2(x, y+ a->speed.y), bkp_vec2(x2, y2 + a->speed.y));
			//if(1 == bkp_geometry_getSegmentsIntersection2(l1, cl->line, &pt) || val == 1)
			if(1 == bkp_geometry_getSegmentsIntersection2(l1, cl->line, &pt))
			{
				stop_it = 1;
			}
		}
	}

	if(1 == stop_it)
	{
		a->motion = BKP_MOTION_STAND;
		a->speed.y = 0;
		a->jumped = 0;
	}

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

