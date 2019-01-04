#ifndef __BKP_GEOMETRY_H_
#define __BKP_GEOMETRY_H_

#include "bkp_struct.h"

/*********************************************************************
 * Defines 
*********************************************************************/

#define BKP_ROTATE_DEFAULT BKP_FALSE
#define BKP_ROTATE_REVERSE BKP_TRUE

/*********************************************************************
 * Type def & enum
*********************************************************************/

/*********************************************************************
 * Macro 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Global 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Functions 
*********************************************************************/

//rotation
BKP_Vec2 bkp_geometry_rotatePoint2D(BKP_Vec2 pts, BKP_Rotate r, int reverse);
int bkp_geometry_rectIntersectPt(BKP_Rect rec1, BKP_Rect rec2); // rec2 = (x,y),(x,y) 
int bkp_geometry_rectIntersectDm(BKP_Rect rec1, BKP_Rect rec2); // rec2 = x,y,w,h
int bkp_geometry_pointInRectPt(BKP_Vec2 pts, BKP_Rect rec);
int bkp_geometry_pointInRectDm(BKP_Vec2 pts, BKP_Rect rec);

//equation affine 
BKP_2Dline bkp_geometry_getAffineEq2(vec2_t pts1, vec2_t pts2); //Attention Les points sont supposes distinct (segfault prone)
BKP_2Dline bkp_geometry_get2DlineFromColinear(BKP_Vec2 pt, BKP_Vec2 col);
BKP_2Dline bkp_geometry_get2DlineFromNormal(BKP_Vec2 pt, BKP_Vec2 col);
int bkp_geometry_getLineIntersection2(BKP_2Dline l1, BKP_2Dline l2, vec2_t *pts); //return 0 for parallele, 1 for sequent, -1 for identical
int bkp_geometry_getSegmentsIntersection2(BKP_2Dline l1, BKP_2Dline l2, vec2_t *pts);//return 0 or 1 
BKP_2Dline bkp_geometry_get2Dline(vec2_t pt1, vec2_t pt2);
BKP_Vec2 bkp_geometry_get2DNormalVect(BKP_2Dline l);				//vertor normal
int bkp_geometry_segment2DinRectangle(BKP_Rect pts, BKP_Rect box);

//printing
void bkp_geometry_printLine(const char * info, BKP_2Dline l);

#endif

