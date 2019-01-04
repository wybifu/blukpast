#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/macro.h"
#include "include/bkp_platform.h"
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

/***************************************************************************
*	Prototypes 	
**************************************************************************/
static void bkp_platform_2DinitPlatform(BKP_Platform2D *p, BKP_Rect rect, BKP_Vec3 rinfo, BKP_PlatformInfo pInfo,void * phyprop);
static int platform_transperse(BKP_Rect pts, BKP_Platform2D *p);

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

BKP_PlatformMap * bkp_plateform_2DcreateMap(void)
{
	BKP_PlatformMap * pm = (BKP_PlatformMap *) bkp_memory_getTicket(sizeof(BKP_PlatformMap));
	pm->left= pm->right = NULL;
	pm->total = 0;
	pm->current = NULL;

	return pm;
}

/*--------------------------------------------------------------------------------*/
void bkp_platform_2DdestroyMap(BKP_PlatformMap * Map)
{
	BKP_Platform2D * p = NULL;

	bkp_platform_2DMap_lrewind(Map);

	while(p = bkp_platform_2DMap_lpop(Map), NULL != p)
		bkp_memory_releaseTicket(p);

	bkp_memory_releaseTicket(Map);
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_platform_2DMap_remove(BKP_PlatformMap * map, BKP_Platform2D * cl)
{
	struct local_membrane_p * mem = (struct local_membrane_p *) cl->membrane;
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
BKP_Platform2D * bkp_platform_2DMap_lpush(BKP_PlatformMap * cm, BKP_Rect pts, BKP_Vec3 rinfo,BKP_PlatformInfo pInfo ,void * phyprop)
{

	struct local_membrane_p * lc = (struct local_membrane_p *) bkp_memory_getTicket(sizeof(struct local_membrane_p));
	lc->platform = (BKP_Platform2D *) bkp_memory_getTicket(sizeof(BKP_Platform2D));
	bkp_platform_2DinitPlatform(lc->platform, pts, rinfo, pInfo, phyprop);

	lc->platform->membrane = (void *) lc;
	lc->selfleft = NULL;
	lc->selfright = cm->left;

	if(NULL != cm->left)
		cm->left->selfleft = lc;
	else
		cm->right = cm->current = lc;

	cm->left = lc;
	cm->total += 1;

	return lc->platform;
}

/*--------------------------------------------------------------------------------*/
BKP_Platform2D * bkp_platform_2DMap_lpop(BKP_PlatformMap * map)
{
	if(NULL == map->left)
		return NULL;

	map->total -= 1;
	struct local_membrane_p *lc = map->left;
	map->left = lc->selfright;
	if(NULL != lc->selfright)
		lc->selfright->selfleft = NULL;

	BKP_Platform2D * platform = lc->platform;
	bkp_memory_releaseTicket(lc);

	return platform;
}

/*--------------------------------------------------------------------------------*/
BKP_Platform2D * bkp_platform_2DMap_rpush(BKP_PlatformMap * cm, BKP_Rect pts, BKP_Vec3 rinfo,BKP_PlatformInfo pInfo ,void * phyprop)
{

	struct local_membrane_p * lc = (struct local_membrane_p *) bkp_memory_getTicket(sizeof(struct local_membrane_p));
	lc->platform = (BKP_Platform2D *) bkp_memory_getTicket(sizeof(BKP_Platform2D));
	bkp_platform_2DinitPlatform(lc->platform, pts, rinfo, pInfo, phyprop);

	lc->platform->membrane = (void *) lc;
	lc->selfright = NULL;
	lc->selfleft= cm->right;

	if(NULL != cm->right)
		cm->right->selfright = lc;
	else
		cm->left = cm->current = lc;

	cm->right = lc;
	cm->total += 1;

	return lc->platform;
}

/*--------------------------------------------------------------------------------*/
BKP_Platform2D * bkp_platform_2DMap_rpop(BKP_PlatformMap * map)
{
	if(NULL == map->right)
		return NULL;

	map->total -= 1;
	struct local_membrane_p *lc = map->right;
	map->left = lc->selfleft;
	if(NULL != lc->selfleft)
		lc->selfright->selfright = NULL;

	BKP_Platform2D * platform = lc->platform;
	bkp_memory_releaseTicket(lc);

	return platform;
}

/*--------------------------------------------------------------------------------*/
void bkp_platform_2DMap_lrewind(BKP_PlatformMap * map)
{
	map->current = map->left;
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_platform_2DMap_rrewind(BKP_PlatformMap * map)
{
	map->current = map->right;
	return;
}

/*--------------------------------------------------------------------------------*/
BKP_Platform2D * bkp_platform_2DMap_lget(BKP_PlatformMap * map)
{
	if(NULL == map->current)
			return NULL;

	BKP_Platform2D * lc = map->current->platform;
	map->current = map->current->selfright;

	return lc;
}

/*--------------------------------------------------------------------------------*/
BKP_Platform2D * bkp_platform_2DMap_rget(BKP_PlatformMap * map)
{
	if(NULL == map->current)
			return NULL;

	BKP_Platform2D * lc = map->current->platform;
	map->current = map->current->selfleft;

	return lc;
}


/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
void bkp_platform_2Dupdate(BKP_Platform2D *p, BKP_Rect rect, float cx, float cy, float angle)
{
	p->dyn.gbox.point = bkp_numcal_vec2(rect.point.x, rect.point.y);
	p->dyn.gbox.dimension = bkp_numcal_vec2(rect.dimension.w, rect.dimension.h);
//	p->bbox_color = bkp_numcal_vec4(0xff, 0xff, 0xff, 0xff);

	p->rotate.center = bkp_vec3(cx, cy, 0.0f);
	p->rotate.angle_deg = angle;
	angle = BKP_NUMCAL_DEG2RAD(angle);
	p->rotate.angle = angle;
	p->rotate.axis = bkp_vec3(0.0f, 0.0f, 1.0f);
	p->rotate.sin_a = sin(angle);
	p->rotate.cos_a = cos(angle);

	if(angle != 0.0f)
	{
		p->isRotated = BKP_TRUE;

		BKP_Vec2 pts;
		BKP_Vec4 xvec, yvec;

		pts = bkp_geometry_rotatePoint2D(p->dyn.pbox.point, p->rotate, BKP_ROTATE_DEFAULT);
		xvec.v[0] = pts.x;
		yvec.v[0] = pts.y;

		pts = bkp_geometry_rotatePoint2D(bkp_vec2(p->dyn.pbox.x, p->dyn.pbox.y + p->dyn.pbox.h), p->rotate, BKP_ROTATE_DEFAULT);
		xvec.v[1] = pts.x;
		yvec.v[1] = pts.y;

		pts = bkp_geometry_rotatePoint2D(bkp_vec2(p->dyn.pbox.x + p->dyn.pbox.w, p->dyn.pbox.y + p->dyn.pbox.h), p->rotate, BKP_ROTATE_DEFAULT);
		xvec.v[2] = pts.x;
		yvec.v[2] = pts.y;

		pts = bkp_geometry_rotatePoint2D(bkp_vec2(p->dyn.pbox.x + p->dyn.pbox.w, p->dyn.pbox.y), p->rotate, BKP_ROTATE_DEFAULT);
		xvec.v[3] = pts.x;
		yvec.v[3] = pts.y;

		p->bbox.point = bkp_numcal_vec2( bkp_numcal_minvec4(xvec), bkp_numcal_minvec4(yvec)); 
		p->bbox.dimension = bkp_numcal_vec2( bkp_numcal_maxvec4(xvec) - p->bbox.x,  bkp_numcal_maxvec4(yvec) - p->bbox.y); 

	}
	else
	{
		p->isRotated = BKP_FALSE;
		p->bbox.point = bkp_numcal_vec2(p->dyn.pbox.x, p->dyn.pbox.y); 
		p->bbox.dimension = bkp_numcal_vec2(p->dyn.pbox.w, p->dyn.pbox.h); 
	}


	return;
}

/*--------------------------------------------------------------------------------*/
int bkp_platform_stillOnPlatform2D(BKP_Dynamics2D * a)
{
	int ok = BKP_FALSE;
	BKP_Platform2D * p = a->cpts[BKP_DOWN].p;
	if(p != NULL)
	{
		BKP_Rect pts;
		pts.pt1 = bkp_vec2(a->gbox.x + a->cpts[BKP_DOWN].pts.pt1.x , a->gbox.y + a->cpts[BKP_DOWN].pts.pt1.y + 1.0f);
		pts.pt2 = bkp_vec2(a->gbox.x + a->cpts[BKP_DOWN].pts.pt2.x , a->gbox.y + a->cpts[BKP_DOWN].pts.pt2.y + 1.0f);
		if(p->isRotated)
		{
			pts.pt1 = bkp_geometry_rotatePoint2D(pts.pt1, p->rotate, BKP_ROTATE_REVERSE);
			pts.pt2 = bkp_geometry_rotatePoint2D(pts.pt2, p->rotate, BKP_ROTATE_REVERSE);
		}

		if(BKP_FALSE == bkp_geometry_segment2DinRectangle(pts, p->dyn.pbox))
		{
			a->cpts[BKP_DOWN].p = NULL;
			a->motion = BKP_MOTION_FALL;
				if(fcmp(a->speed.y, 0.0f) >= 0)
					a->jumped = 1;
		}
		else
		{
			a->motion = BKP_MOTION_STAND;
			a->jumped = 0;
			ok = BKP_TRUE;
		}
	}
	
	return ok;
}

/*--------------------------------------------------------------------------------*/
int bkp_platform_2DgetBounds(BKP_Rect rect, BKP_PlatformMap * Map, BKP_Platform2D ** res)
{
	int k = 0;
	BKP_Platform2D *p = NULL;

	bkp_platform_2DMap_lrewind(Map);
	while(p = bkp_platform_2DMap_lget(Map), NULL != p)
	{
		p->contact = BKP_FALSE;

		if(BKP_FALSE == bkp_geometry_rectIntersectDm(rect, p->bbox)) continue;

		res[k] = p;
		++k;
		p->contact = BKP_TRUE;
	}
	return k;
}

/*--------------------------------------------------------------------------------*/
int bkp_platform_2DhasBoundsCollision(BKP_Rect obj, BKP_Platform2D Map)
{
	return bkp_geometry_rectIntersectDm(obj, Map.bbox);
}

/*--------------------------------------------------------------------------------*/
BKP_Platform2D * bkp_platform_2DejectFromPlatform(BKP_Rect * pts, BKP_Platform2D * Map)
{
	BKP_Rect p;
	p.x = Map->dyn.pbox.x;
	p.y = Map->dyn.pbox.y;
	p.w = Map->dyn.pbox.w;
	p.h = Map->dyn.pbox.h;

	if(Map->fblock[BKP_UP])
		if(bkp_platform_intersectSegmentFace2D(pts, p, Map->rotate, BKP_UP) == BKP_TRUE)
			return Map;

	if(Map->fblock[BKP_LEFT])
		if(bkp_platform_intersectSegmentFace2D(pts, p, Map->rotate, BKP_LEFT) == BKP_TRUE)
			return Map;

	p.y = Map->dyn.pbox.y + Map->dyn.pbox.h;
	if(Map->fblock[BKP_DOWN])
		if(bkp_platform_intersectSegmentFace2D(pts, p, Map->rotate, BKP_DOWN) == BKP_TRUE)
			return Map;

	p.x = Map->dyn.pbox.x + Map->dyn.pbox.w;
	p.y = Map->dyn.pbox.y;
	if(Map->fblock[BKP_RIGHT])
		if(bkp_platform_intersectSegmentFace2D(pts, p, Map->rotate, BKP_RIGHT) == BKP_TRUE)
			return Map;

	return NULL;
}

/*--------------------------------------------------------------------------------*/
int bkp_platform_intersectSegmentFace2D(BKP_Rect * obj, BKP_Rect p, BKP_Rotate r, int face)
{
	BKP_Rect tmp;
	tmp.pt1 = bkp_geometry_rotatePoint2D(obj->pt1, r, BKP_ROTATE_REVERSE);
	tmp.pt2 = bkp_geometry_rotatePoint2D(obj->pt2, r, BKP_ROTATE_REVERSE);

	//point1 outside (<= >=) point2 inside (< >)
	if((face == BKP_UP && fcmp(tmp.y1, p.y) <= 0 && fcmp(tmp.y2, p.y) > 0) || (face == BKP_DOWN && fcmp(tmp.y1, p.y) >= 0 && fcmp(tmp.y2, p.y) < 0))
	{
		float x = tmp.x1 + (p.y - tmp.y1) / (tmp.y2 - tmp.y1) * (tmp.x2 - tmp.x1);
		if((fcmp(x, p.x) > 0 && fcmp(x, p.x + p.w) < 0) || 
			(fcmp(x, p.x) == 0 && fcmp(tmp.x1, tmp.x2) < 0) || 		// < we know the left face will miss it 
			(fcmp(x, p.x + p.w) == 0 && fcmp(tmp.x1, tmp.x2) > 0)) // > we know the right face will miss it 
		{
			BKP_Vec2 pts;
			if(face == BKP_UP)
				pts = bkp_numcal_vec2(x, p.y);
			else
				pts = bkp_numcal_vec2(x, p.y);

			obj->pt2 = bkp_geometry_rotatePoint2D(pts, r, BKP_ROTATE_DEFAULT);

			return BKP_TRUE;
		}
	}
	else if((face == BKP_LEFT && fcmp(tmp.x1, p.x) <= 0 && fcmp(tmp.x2, p.x) > 0) || (face == BKP_RIGHT && fcmp(tmp.x1, p.x) >= 0 && fcmp(tmp.x2, p.x) < 0))
	{
		float y = tmp.y1 + (p.x - tmp.x1) / (tmp.x2 - tmp.x1) * (tmp.y2 - tmp.y1);
		if((fcmp(y, p.y) > 0 && fcmp(y, p.y + p.h) < 0) || 
			(fcmp(y, p.y) == 0 && fcmp(tmp.y1, tmp.y2) < 0) || 		// < we know the up face will miss it 
			(fcmp(y, p.y + p.h) == 0 && fcmp(tmp.y1, tmp.y2) > 0)) // > we know the down face will miss it 
		{
			BKP_Vec2 pts;
			if(face == BKP_LEFT)
				pts = bkp_numcal_vec2(p.x, y);
			else
				pts = bkp_numcal_vec2(p.x, y);

			obj->pt2 = bkp_geometry_rotatePoint2D(pts, r, BKP_ROTATE_DEFAULT);

			return BKP_TRUE;
		}
	}

	return BKP_FALSE;
}

/*--------------------------------------------------------------------------------*/
int bkp_platform_2DsegmentIntersect(BKP_Rect * pts1, BKP_Rect * pts2, BKP_Platform2D * Map)
{
	BKP_Vec2 ori1  = bkp_geometry_rotatePoint2D(pts1->pt1, Map->rotate, BKP_ROTATE_REVERSE); //origine
	BKP_Vec2 ori2  = bkp_geometry_rotatePoint2D(pts2->pt1, Map->rotate, BKP_ROTATE_REVERSE);
	BKP_Vec2 pt1   = bkp_geometry_rotatePoint2D(pts1->pt2, Map->rotate, BKP_ROTATE_REVERSE);	// destination
	BKP_Vec2 pt2   = bkp_geometry_rotatePoint2D(pts2->pt2, Map->rotate, BKP_ROTATE_REVERSE);
	BKP_2Dline l   = bkp_geometry_getAffineEq2(pt1, pt2);
	BKP_2Dline eqp = bkp_geometry_getAffineEq2(bkp_vec2(pt2.x + 1, pt2.y), pt2); 
	vec2_t p;

	int val = bkp_geometry_getLineIntersection2(l, eqp, &p);
	if(val == 1)
	{
		if(p.x >= Map->dyn.pbox.x && p.x <= Map->dyn.pbox.x + Map->dyn.pbox.w)
		{
			if(ori1.x <= Map->dyn.pbox.x || ori2.x <= Map->dyn.pbox.x)
			{
				float dx = p.x - Map->dyn.pbox.x;
				pts1->pt2.x -= dx - 1;
				pts2->pt2.x -= dx - 1;
				fprintf(stdout,"back left of %.4f\n",dx);
			}
			else if(ori1.x > Map->dyn.pbox.x + Map->dyn.pbox.w || ori2.x > Map->dyn.pbox.x + Map->dyn.pbox.w)
			{
				float dx = (Map->dyn.pbox.x + Map->dyn.pbox.w) - p.x;
				pts1->pt2.x += dx + 1;
				pts2->pt2.x += dx + 1;
				fprintf(stdout,"back right of %.4f\n", dx);
			}

			return BKP_TRUE;
		}
	}

	return BKP_FALSE;
}
/*--------------------------------------------------------------------------------*/
int bkp_platform_removeFromPlatform2D(BKP_Dynamics2D * a, BKP_Platform2D * p, float * movx, float * movy)
{
	int to_return = BKP_FALSE;
	float newx = *movx,
		  newy = *movy;

	if(BKP_TRUE == p->physprop->allowin)
		return to_return;

	for(int i = 0; i < 8; ++i)
	{
		BKP_Vec2 pts = bkp_vec2(a->gbox.x + a->cpts[i].pts.x + newx, a->gbox.y + a->cpts[i].pts.y + newy);
		pts = bkp_geometry_rotatePoint2D(pts, p->rotate, BKP_ROTATE_REVERSE);
		
		if(BKP_FALSE == bkp_geometry_pointInRectDm(pts, p->dyn.pbox)) continue;

		to_return = BKP_TRUE;

		float d = pts.y - p->dyn.pbox.y;
		float dx = 0,
			  dy = -d;

		float tmpd = p->dyn.pbox.y + p->dyn.pbox.h - pts.y;
		if(tmpd < d) dy = d = tmpd;

		tmpd = pts.x - p->dyn.pbox.x;
		if(tmpd < d)
		{
			dx = -(d = tmpd);
			dy = 0;
		}

		tmpd = p->dyn.pbox.x + p->dyn.pbox.w - pts.x;
		if(tmpd < d)
		{
			dx = tmpd;
			dy = 0;
		}

		pts.x += dx;
		pts.y += dy;

		pts = bkp_geometry_rotatePoint2D(pts, p->rotate, BKP_ROTATE_DEFAULT);

		newx = pts.x - a->gbox.x - a->cpts[i].pts.x;
		newy = pts.y - a->gbox.y - a->cpts[i].pts.y;
		
	}

	 *movx = newx;
	 *movy = newy;

	return to_return;
}

/*--------------------------------------------------------------------------------*/
int bkp_platform_inPlatform2D(BKP_Dynamics2D * a, BKP_Platform2D ** p, int n, float movx, float movy)
{
	for(int o = 0; o < n; ++o)
	{
		for(int i = 0; i < 8; ++i)
		{
			BKP_Vec2 pts = bkp_vec2(a->gbox.x + a->cpts[i].pts.x + movx, a->gbox.y + a->cpts[i].pts.y + movy);
			pts = bkp_geometry_rotatePoint2D(pts, p[o]->rotate, BKP_ROTATE_REVERSE);

			if(BKP_TRUE == bkp_geometry_pointInRectDm(pts, p[o]->dyn.pbox))
				return BKP_TRUE;
		}
	}

	return BKP_FALSE;
}

/*--------------------------------------------------------------------------------*/
int bkp_platform_isPointInPlatform2D(BKP_Vec2 point, BKP_Platform2D * p)
{
	BKP_Vec2 pts; 

	if(BKP_TRUE == p->isRotated)
		pts = bkp_geometry_rotatePoint2D(point, p->rotate, BKP_ROTATE_REVERSE);
	else
		pts = bkp_vec2(point.x, point.y);

	return bkp_geometry_pointInRectDm(pts, p->dyn.pbox);
}

/*--------------------------------------------------------------------------------*/
BKP_Collision2DReturn bkp_platform_2Dcollision(BKP_Dynamics2D * a, BKP_PlatformMap * Map, BKP_Vec2 mov)
{ 
	BKP_Platform2D * mbounds[Map->total];
	int nbounds, 
		collides = BKP_FALSE,
		c_dir[BKP_2D_MAX_DIR] = {BKP_FALSE};
	float newx = mov.x,
		  newy = mov.y;

	BKP_Rect player_bound = bkp_rect(
			bkp_numcal_min2f(a->gbox.x + a->pbox.x, a->gbox.x + a->pbox.x + newx),
			bkp_numcal_min2f(a->gbox.y + a->pbox.y, a->gbox.y + a->pbox.y + newy),
			bkp_numcal_max2f(a->gbox.x + a->pbox.x + a->pbox.w, a->gbox.x + a->pbox.x + a->pbox.w + newx),
			bkp_numcal_max2f(a->gbox.y + a->pbox.y + a->pbox.h, a->gbox.y + a->pbox.y + a->pbox.h + newy));

	BKP_Collision2DReturn result = bkp_createCollision2DReturn(mov);
	nbounds = bkp_platform_2DgetBounds(player_bound, Map, mbounds);

	BKP_Rect colpts[BKP_2D_MAX_DIR];
	colpts[BKP_UP] 	  = bkp_rect(a->gbox.x + a->cpts[BKP_UP].pts.pt1.x, a->gbox.y + a->cpts[BKP_UP].pts.pt1.y,
			a->gbox.x + a->cpts[BKP_UP].pts.pt2.x, a->gbox.y + a->cpts[BKP_UP].pts.pt2.y);
	colpts[BKP_LEFT]  = bkp_rect(a->gbox.x + a->cpts[BKP_LEFT].pts.pt1.x, a->gbox.y + a->cpts[BKP_LEFT].pts.pt1.y,
			a->gbox.x + a->cpts[BKP_LEFT].pts.pt2.x, a->gbox.y + a->cpts[BKP_LEFT].pts.pt2.y);
	colpts[BKP_DOWN]  = bkp_rect(a->gbox.x + a->cpts[BKP_DOWN].pts.pt1.x, a->gbox.y + a->cpts[BKP_DOWN].pts.pt1.y,
			a->gbox.x + a->cpts[BKP_DOWN].pts.pt2.x, a->gbox.y + a->cpts[BKP_DOWN].pts.pt2.y);
	colpts[BKP_RIGHT] = bkp_rect(a->gbox.x + a->cpts[BKP_RIGHT].pts.pt1.x, a->gbox.y + a->cpts[BKP_RIGHT].pts.pt1.y,
			a->gbox.x + a->cpts[BKP_RIGHT].pts.pt2.x, a->gbox.y + a->cpts[BKP_RIGHT].pts.pt2.y);

	int contact = BKP_TRUE;
	a->motion = BKP_MOTION_FALL;


	for(int iteration = 0; iteration < stc_collision_iterations && contact; ++iteration)
	{
		contact = BKP_FALSE;
		for(int o = 0; o < nbounds; ++o)
		{
			if(bkp_platform_2DhasBoundsCollision(player_bound, *mbounds[o]) == BKP_FALSE)
				continue;

			float vectorLength = sqrtf(newx * newx + newy * newy);

			if(vectorLength == 0) continue; //avoid tmpx or tmpy = NaN

			int hasFloating = fcmp(vectorLength, (float) (int) vectorLength) > 0 ? BKP_TRUE : BKP_FALSE;
			int segments = (int) vectorLength + (BKP_TRUE == hasFloating ? 1 : 0);
			float tmpx = newx / vectorLength,
				  tmpy = newy / vectorLength;

			for(int iseg = 1; iseg <= segments && BKP_FALSE == contact; ++iseg)
			{
				for(int dir = 0; dir < BKP_2D_MAX_DIR; ++dir)
				{
					if(dir == BKP_UP    && newy >= 0) continue;
					if(dir == BKP_LEFT  && newx >= 0) continue;
					if(dir == BKP_DOWN  && newy <= 0) continue;
					if(dir == BKP_RIGHT && newx <= 0) continue;

					BKP_Rect pts1,pts2;
					BKP_Platform2D * pts_col1 = NULL,
								   * pts_col2 = NULL;

					pts1.pt1 = colpts[dir].pt1;
					pts1.pt2 = bkp_vec2(pts1.pt1.x + iseg * tmpx, pts1.pt1.y + iseg * tmpy);
					pts2.pt1 = colpts[dir].pt2;
					pts2.pt2 = bkp_vec2(pts2.pt1.x + iseg * tmpx, pts2.pt1.y + iseg * tmpy);


					pts_col1 = bkp_platform_2DejectFromPlatform(&pts1, mbounds[o]);
					pts_col2 = bkp_platform_2DejectFromPlatform(&pts2, mbounds[o]);

					if(!pts_col1 && !pts_col2 )
					{ 
						BKP_Rect pts = bkp_rect(pts1.pt2.x, pts1.pt2.y, pts2.pt2.x, pts2.pt2.y);
						if(BKP_FALSE == platform_transperse(pts, mbounds[o]))
							continue;
						else
						{
							pts_col1 = mbounds[o];
						}
					}

					a->cpts[dir].p = pts_col1 != NULL ? pts_col1 : pts_col2;

					contact = BKP_TRUE;
					collides = BKP_TRUE;
					c_dir[dir] = BKP_TRUE;

					if(dir == BKP_LEFT || dir == BKP_RIGHT)
					{
						a->speed.x = 0.0f;
						newx = (iseg - 1) * tmpx;
					}
					else
					{
						a->speed.y = 0.0f;
						newy = (iseg - 1) * tmpy;
						if(dir == BKP_DOWN)
						{
							a->motion = BKP_MOTION_STAND;
							a->jumped= 0;
						}
					}
				}
			}
		}
	}

	if(BKP_TRUE == collides)
	{
		result.mov 				= bkp_vec2(newx, newy);
		//result.inside 			= bkp_platform_inPlatform2D(a, mbounds, nbounds, newx, newy);
		result.inside 			= BKP_FALSE;
		result.collides			= collides;
		for(int i = 0 ; i< BKP_2D_MAX_DIR; ++i)
			result.dirs[i] = c_dir[i];
		if(BKP_FALSE == c_dir[BKP_LEFT])
			a->cpts[BKP_LEFT].p = NULL;
		if(BKP_FALSE == c_dir[BKP_RIGHT])
			a->cpts[BKP_RIGHT].p = NULL;

		//fprintf(stderr,"%s = %d %p\n",g_bkp_strdir[BKP_RIGHT],result.dirs[BKP_RIGHT], a->cpts[BKP_RIGHT].p);
	}

	return result;
}

/*--------------------------------------------------------------------------------*/
BKP_Vec2 bkp_platform_horizontalMove(BKP_Dynamics2D * dyn)
{
	BKP_Vec2 mx = bkp_vec2(0.0f, 0.0f);
	int dir = dyn->dir;

	if(dyn->motion == BKP_MOTION_STAND)
	{
		BKP_Platform2D * floor = dyn->cpts[BKP_DOWN].p;
		BKP_Platform2D * wall = dyn->cpts[dir].p;

		if(floor == NULL)
			return mx;

		if(wall != NULL)
		{
			float y = dyn->gbox.y + dyn->cpts[dir].pts.pt1.y;
			if(y < wall->dyn.pbox.y) 
			{
				float ystair = dyn->gbox.y + dyn->climbpts.y,
					  yclimb = dyn->gbox.y + dyn->climbpts.x; 
				if(wall->dyn.pbox.y > ystair)
				{
					if(dir == BKP_RIGHT)
					{
						dyn->gbox.x += 1;
						dyn->speed.x = dyn->stairsSpeed;
					}
					else
					{
						dyn->gbox.x -= 1;
						dyn->speed.x = -dyn->stairsSpeed;
					}
					dyn->gbox.y += wall->dyn.pbox.y - (dyn->gbox.y + dyn->cpts[BKP_DOWN].pts.y);
					/*
					dyn->onStairs = BKP_TRUE;
					dyn->stairs = bkp_vec2(0.0f, wall->dyn.pbox.y);
					*/
				}
				else if(wall->dyn.pbox.y > yclimb)
				{

				//	fprintf(stderr,"should escalade %p\n",wall);
				}
				else
				{
					dyn->speed.y = -dyn->jmp_force; // should be difference of y to be able to grap it
					dyn->jumped = 1;
					fprintf(stderr,"Man jump if you want to go\n");
				}
			}
			else
			{
				dyn->maxSpeed.x = dyn->dmaxSpeed.x - dyn->dmaxSpeed.x * floor->physprop->sticky;
				mx.x = dyn->force.x * (1 - floor->physprop->sticky);
			}
		}
		else
		{
			dyn->maxSpeed.x = dyn->dmaxSpeed.x - dyn->dmaxSpeed.x * floor->physprop->sticky;
			mx.x = dyn->force.x * (1 - floor->physprop->sticky);

			if(BKP_TRUE == floor->isRotated)
			{
				BKP_Vec2 pts = dyn->gbox.point;
				//			fprintf(stderr,"pts(%.4f,%.4f) mx(%.4f, %.4f) -> ", pts.x, pts.y,mx, *Y);
				pts = bkp_geometry_rotatePoint2D(pts, floor->rotate, BKP_ROTATE_REVERSE);
				pts.x += mx.x;
				pts = bkp_geometry_rotatePoint2D(pts, floor->rotate, BKP_ROTATE_DEFAULT);
				mx.x = pts.x - dyn->gbox.x;
				mx.y = pts.y - dyn->gbox.y;
				//			fprintf(stderr,"pts(%.4f,%.4f) mx(%.4f, %.4f)\n", pts.x, pts.y,mx, *Y);
			}
		}
	}
	mx.x = (BKP_RIGHT == dir) ? mx.x : -mx.x ;
	return mx; 
}

/*--------------------------------------------------------------------------------*/
void bkp_platform_2DonStairs(BKP_Dynamics2D * d, float force)
{
	d->speed.y -= force;
	float y = d->cpts[BKP_RIGHT].pts.pt2.y + d->gbox.y;

	if(d->dir == BKP_LEFT)
		d->speed.x -= 5;
	else
		d->speed.x += 5;

	if(y <= d->stairs.y - 12)
	{
		d->speed.y = 0;
		d->onStairs = BKP_FALSE;
	}

	return;
}

/*--------------------------------------------------------------------------------*/
float bkp_platform_horizontalStop(BKP_Dynamics2D dyn)
{
	float mx = 0.0;

	if(dyn.motion == BKP_MOTION_FLOAT)
		mx = 0.0;
	else if(dyn.motion == BKP_MOTION_STAND)
	{
		BKP_Platform2D * p = NULL;
		if((p = dyn.cpts[2 * BKP_DOWN].p) == NULL)
			if((p = dyn.cpts[2 * BKP_DOWN + 1].p) == NULL)
				return mx;

		mx = dyn.dec.x / (1 - p->physprop->sticky);

		if(dyn.speed.x < 0 && dyn.speed.x + mx > 0)
			mx =  -dyn.speed.x;
		if(dyn.speed.x > 0 && dyn.speed.x - mx < 0)
			mx =  dyn.speed.x;
	}
	return mx;
}

/*--------------------------------------------------------------------------------*/
float bkp_platform_slider(BKP_Dynamics2D * dyn)
{
	BKP_Platform2D * p = NULL;

	dyn->sliding = BKP_FALSE;
	if((p = dyn->cpts[2 * BKP_DOWN].p) == NULL)
		if((p = dyn->cpts[2 * BKP_DOWN + 1].p) == NULL)
			return 0.0f;
	if(0 != p->physprop->slide)
		dyn->sliding = BKP_TRUE;
	return p->physprop->slide;
}

/*--------------------------------------------------------------------------------*/
BKP_Rect bkp_platform_get2DWoldBound(BKP_PlatformMap * Map, BKP_Vec2 spading)
{
	BKP_Platform2D * p = NULL;
	float x1 , y1 ,  x2 , y2;
	x1 = x2 = y1 = y2 = 0.0f;

	bkp_platform_2DMap_lrewind(Map);
	while(p = bkp_platform_2DMap_lget(Map), NULL != p)
	{

		if(p->bbox.x <= x1)
			x1 = p->bbox.x;
		if(p->bbox.x >= x2)
			x2 = p->bbox.x;
		if(p->bbox.y <= y1)
			y1 = p->bbox.y;
		if(p->bbox.y >= y2)
			y2 = p->bbox.y;

	}
	return bkp_rect(x1, y1, x2 + spading.x, y2 + spading.y);
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

static void bkp_platform_2DinitPlatform(BKP_Platform2D *p, BKP_Rect rect, BKP_Vec3 rinfo, BKP_PlatformInfo pInfo,void * phyprop)
{
	p->dyn.pbox.point = rect.point;
	p->dyn.pbox.dimension = rect.dimension;

	p->color = pInfo.color;
	p->contact = BKP_FALSE;
	if(phyprop)
		p->physprop = phyprop;
	else
		p->physprop = PlatformMaterial[0];
	
	p->visible = pInfo.visible;
	p->visible_fill = pInfo.visible_fill;

	for(int i = 0; i < 4; ++i)
	{
		p->fblock[i] = pInfo.face_flag & 0x1;
		pInfo.face_flag >>=  1;
	}

	if(fabs(rinfo.z) >= 360)
	{
		int an 	= rinfo.z / 360;
		int mod	= (int)rinfo.z % 360;
		rinfo.z	= mod + (rinfo.z - (an * 360 + mod));
	}

	p->rotate.center = bkp_vec3(rinfo.x, rinfo.y, 0.0f);
	p->rotate.angle_deg = rinfo.z;

	float angle = BKP_NUMCAL_DEG2RAD(rinfo.z);
	p->rotate.angle = angle;
	p->rotate.axis = bkp_vec3(0.0f, 0.0f, 1.0f);
	p->rotate.sin_a = sin(angle);
	p->rotate.cos_a = cos(angle);

	if(angle != 0.0f)
	{
		p->isRotated = BKP_TRUE;

		BKP_Vec2 pts;
		BKP_Vec4 xvec, yvec;

		pts = bkp_geometry_rotatePoint2D(p->dyn.pbox.point, p->rotate, BKP_ROTATE_DEFAULT);
		xvec.v[0] = pts.x;
		yvec.v[0] = pts.y;

		pts = bkp_geometry_rotatePoint2D(bkp_vec2(p->dyn.pbox.x, p->dyn.pbox.y + p->dyn.pbox.h), p->rotate, BKP_ROTATE_DEFAULT);
		xvec.v[1] = pts.x;
		yvec.v[1] = pts.y;

		pts = bkp_geometry_rotatePoint2D(bkp_vec2(p->dyn.pbox.x + p->dyn.pbox.w, p->dyn.pbox.y + p->dyn.pbox.h), p->rotate, BKP_ROTATE_DEFAULT);
		xvec.v[2] = pts.x;
		yvec.v[2] = pts.y;

		pts = bkp_geometry_rotatePoint2D(bkp_vec2(p->dyn.pbox.x + p->dyn.pbox.w, p->dyn.pbox.y), p->rotate, BKP_ROTATE_DEFAULT);
		xvec.v[3] = pts.x;
		yvec.v[3] = pts.y;

		p->bbox.point = bkp_numcal_vec2( bkp_numcal_minvec4(xvec), bkp_numcal_minvec4(yvec)); 
		p->bbox.dimension = bkp_numcal_vec2( bkp_numcal_maxvec4(xvec) - p->bbox.x,  bkp_numcal_maxvec4(yvec) - p->bbox.y); 

	}
	else
	{
		p->isRotated = BKP_FALSE;
		p->bbox.point = bkp_numcal_vec2(p->dyn.pbox.x, p->dyn.pbox.y); 
		p->bbox.dimension = bkp_numcal_vec2(p->dyn.pbox.w, p->dyn.pbox.h); 
	}

	return;
}

/*--------------------------------------------------------------------------------*/
static int platform_transperse(BKP_Rect pts, BKP_Platform2D *p)
{
	BKP_2Dline l1;

	if(BKP_TRUE == p->isRotated)
	{
		pts.pt1 = bkp_geometry_rotatePoint2D(pts.pt1, p->rotate, BKP_ROTATE_REVERSE);
		pts.pt2 = bkp_geometry_rotatePoint2D(pts.pt2, p->rotate, BKP_ROTATE_REVERSE);
	}

	l1 = bkp_geometry_get2Dline(pts.pt1, pts.pt2);

	return bkp_geometry_segment2DinRectangle(pts, p->dyn.pbox);

	fprintf(stderr,"should never appear\n");

	if( BKP_TRUE == l1.isXaxis && 
			(l1.pt1.x > p->dyn.pbox.x && l1.pt1.x < (p->dyn.pbox.x + p->dyn.pbox.w)) &&
			!(l1.pt2.y < p->dyn.pbox.y || l1.pt1.y > p->dyn.pbox.y + p->dyn.pbox.h))
		return BKP_TRUE;

	if(0 == l1.eq.a_ &&
			(l1.pt1.y > p->dyn.pbox.y && l1.pt1.y < (p->dyn.pbox.y + p->dyn.pbox.h)) &&
			!(l1.pt2.x < p->dyn.pbox.x || l1.pt1.x > p->dyn.pbox.x + p->dyn.pbox.w))
		return BKP_TRUE;

	/*
	val1 = bkp_geometry_getSegmentsIntersection2(l1, cl->line, &pt1);
	if(val1 == 1)
		return BKP_TRUE;
		*/

	return BKP_FALSE;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

