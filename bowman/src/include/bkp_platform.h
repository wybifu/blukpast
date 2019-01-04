#ifndef __BKP_PLATFORM_H_
#define __BKP_PLATFORM_H_

#include "bkp_struct.h"
#include "bkp_material.h"

/*********************************************************************
 * Defines 
*********************************************************************/

/*********************************************************************
 * Type def & enum
*********************************************************************/

/*********************************************************************
 * Macro 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/
struct local_membrane_p
{
	BKP_Platform2D * platform;
	struct local_membrane_p * selfleft;
	struct local_membrane_p * selfright;
};

typedef struct BKP_PlatformMap
{
	struct local_membrane_p * left;
	struct local_membrane_p * right;

	struct local_membrane_p * current;

	int total;

}BKP_PlatformMap;

/*********************************************************************
 * Global 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Functions 
*********************************************************************/

BKP_PlatformMap * bkp_plateform_2DcreateMap(void);
void bkp_platform_2DdestroyMap(BKP_PlatformMap * Map);
void bkp_platform_2DMap_remove(BKP_PlatformMap * map, BKP_Platform2D * cl);

BKP_Platform2D * bkp_platform_2DMap_lpush(BKP_PlatformMap * cm, BKP_Rect pts, BKP_Vec3 rinfo,BKP_PlatformInfo pInfo ,void * phyprop);
BKP_Platform2D * bkp_platform_2DMap_lpop(BKP_PlatformMap * map);

BKP_Platform2D * bkp_platform_2DMap_rpush(BKP_PlatformMap * cm, BKP_Rect pts, BKP_Vec3 rinfo,BKP_PlatformInfo pInfo ,void * phyprop);
BKP_Platform2D * bkp_platform_2DMap_rpop(BKP_PlatformMap * map);

void bkp_platform_2DMap_lrewind(BKP_PlatformMap * map);
void bkp_platform_2DMap_rrewind(BKP_PlatformMap * map);

BKP_Platform2D * bkp_platform_2DMap_lget(BKP_PlatformMap * map);
BKP_Platform2D * bkp_platform_2DMap_rget(BKP_PlatformMap * map);

void bkp_platform_2Dupdate(BKP_Platform2D *p, BKP_Rect rect, float x, float y, float angle);
int bkp_platform_2DgetBounds(BKP_Rect rect, BKP_PlatformMap * plt, BKP_Platform2D ** res);
int bkp_platform_stillOnPlatform2D(BKP_Dynamics2D * a);
int bkp_platform_2DhasBoundsCollision(BKP_Rect obj, BKP_Platform2D plt);
BKP_Platform2D * bkp_platform_2DejectFromPlatform(BKP_Rect * pts, BKP_Platform2D * plt);
int bkp_platform_intersectSegmentFace2D(BKP_Rect * obj, BKP_Rect p, BKP_Rotate r, int face);
int bkp_platform_2DsegmentIntersect(BKP_Rect * pts1, BKP_Rect * pts2, BKP_Platform2D * plt);
int bkp_platform_removeFromPlatform2D(BKP_Dynamics2D * a, BKP_Platform2D *p, float * movx, float * movy);
int bkp_platform_inPlatform2D(BKP_Dynamics2D * a, BKP_Platform2D ** p, int n, float movx, float movy);
int bkp_platform_isPointInPlatform2D(BKP_Vec2 point, BKP_Platform2D * p);
BKP_Collision2DReturn bkp_platform_2Dcollision(BKP_Dynamics2D * a, BKP_PlatformMap * Map, BKP_Vec2 mov);
BKP_Vec2 bkp_platform_horizontalMove(BKP_Dynamics2D * dyn);
void bkp_platform_2DonStairs(BKP_Dynamics2D * d, float force);
float bkp_platform_horizontalStop(BKP_Dynamics2D dyn);
float bkp_platform_slider(BKP_Dynamics2D * dyn);
BKP_Rect bkp_platform_get2DWoldBound(BKP_PlatformMap * Map, BKP_Vec2 spading);



#endif

