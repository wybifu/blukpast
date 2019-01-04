#ifndef __COLLIDER_H_
#define __COLLIDER_H_

#include "bkp_struct.h"

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
struct local_membrane_l
{
	BKP_CollisionLine2D *collision_line;
	struct local_membrane_l * selfleft;
	struct local_membrane_l * selfright;
};


typedef struct BKP_ColliderMap
{
	struct local_membrane_l * left;
	struct local_membrane_l * right;

	struct local_membrane_l * current;
	
	int total;

}BKP_ColliderMap;

/*********************************************************************
 * Global 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Functions 
*********************************************************************/

BKP_ColliderMap * bkp_collider_2DcreateMap(void);
void bkp_collider_2DdestroyMap(BKP_ColliderMap * Map);
void bkp_collider_2DMap_remove(BKP_ColliderMap * map, BKP_CollisionLine2D * cl);

BKP_CollisionLine2D * bkp_collider_2DMap_lpush(BKP_ColliderMap * cm, BKP_Rect pts, BKP_Vec3 rinfo, void * phyprop);
BKP_CollisionLine2D * bkp_collider_2DMap_lpop(BKP_ColliderMap * map);

BKP_CollisionLine2D * bkp_collider_2DMap_rpush(BKP_ColliderMap * cm, BKP_Rect pts, BKP_Vec3 rinfo, void * phyprop);
BKP_CollisionLine2D * bkp_collider_2DMap_rpop(BKP_ColliderMap * map);

BKP_CollisionLine2D * bkp_collider_2DMap_lget(BKP_ColliderMap * map);
BKP_CollisionLine2D * bkp_collider_2DMap_rget(BKP_ColliderMap * map);

void  bkp_collider_2DMap_lrewind(BKP_ColliderMap * map);
void  bkp_collider_2DMap_rrewind(BKP_ColliderMap * map);

BKP_Rect bkp_collider_getWorldBound2D(BKP_ColliderMap * map);
int bkp_collider_2DgetColliderProximity(BKP_ColliderMap * Map,BKP_CollisionLine2D ** res,BKP_Rect player_bound);
BKP_Collision2DReturn bkp_collider_2Dcollision(BKP_Dynamics2D * a, BKP_ColliderMap * Map, BKP_Vec2 mov);
int bkp_collider_motionSurfaceIntersect(BKP_Rect origin, BKP_Rect pos, BKP_CollisionLine2D * cl);
int bkp_collider_ptsNotransperse(BKP_Vec2 opt, BKP_2Dline l1, BKP_2Dline cl, BKP_Vec2 pt1, BKP_Vec2 * diff);
int bkp_collider_horizontalMove(BKP_Dynamics2D *dyn, int dir);
void bkp_collider_agenton(BKP_Dynamics2D *a);

#endif

