#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_geometry.h"

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

BKP_Vec2 bkp_geometry_rotatePoint2D(BKP_Vec2 pts, BKP_Rotate r, int reverse)
{
	BKP_Vec2 tmp;
	float	X = pts.x - r.center.x,
			Y = pts.y - r.center.y;

	if(reverse == BKP_ROTATE_DEFAULT)
	{
		tmp.x = r.center.x + X * r.cos_a - Y * r.sin_a;
		tmp.y = r.center.y + X * r.sin_a + Y * r.cos_a;
	}
	else
	{
		tmp.x = r.center.x + X * r.cos_a  - Y * (- r.sin_a);
		tmp.y = r.center.y + X * (- r.sin_a) + Y * r.cos_a;
	}

	return tmp;
}


/*--------------------------------------------------------------------------------*/
int bkp_geometry_rectIntersectPt(BKP_Rect rec1, BKP_Rect rec2)
{
	if(fcmp(rec1.x1, rec2.x2) >= 0 || fcmp(rec1.x2, rec2.x1) <= 0) return BKP_FALSE;
	if(fcmp(rec1.y1, rec2.y2) >= 0 || fcmp(rec1.y2, rec2.y1) <= 0) return BKP_FALSE;

	return BKP_TRUE;
}

/*--------------------------------------------------------------------------------*/
int bkp_geometry_rectIntersectDm(BKP_Rect rec1, BKP_Rect rec2)
{
	if(fcmp(rec1.x1, rec2.x + rec2.w) >= 0 || fcmp(rec1.x2, rec2.x) <= 0) return BKP_FALSE;
	if(fcmp(rec1.y1, rec2.y + rec2.h) >= 0 || fcmp(rec1.y2, rec2.y) <= 0) return BKP_FALSE;

	return BKP_TRUE;
}

/*--------------------------------------------------------------------------------*/
int bkp_geometry_pointInRectPt(BKP_Vec2 pts, BKP_Rect rec)
{
	if(fcmp(pts.x, rec.x1) > 0 && fcmp(pts.x, rec.x2) < 0 && fcmp(pts.y, rec.y1) > 0 && fcmp(pts.y, rec.y2) < 0)
		return BKP_TRUE;
	return BKP_FALSE;
}

/*--------------------------------------------------------------------------------*/
int bkp_geometry_pointInRectDm(BKP_Vec2 pts, BKP_Rect rec)
{
	if(fcmp(pts.x, rec.x) > 0 && fcmp(pts.x, rec.x + rec.w) < 0 && fcmp(pts.y, rec.y) > 0 && fcmp(pts.y, rec.y + rec.h) < 0)
		return BKP_TRUE;
	return BKP_FALSE;
}

/*--------------------------------------------------------------------------------*/
BKP_2Dline bkp_geometry_getAffineEq2(vec2_t pts1, vec2_t pts2) //Attention Les points sont supposes distincts (segfault prone)
{
	
	BKP_2Dline r;

	r.pt1 = pts1;
	r.pt2 = pts2;

	r.colinear.x = pts2.x - pts1.x;
	r.colinear.y = pts2.y - pts1.y;
	r.normal.x = -r.colinear.y;
	r.normal.y = r.colinear.x;

	if(0 == r.colinear.x)
	{
		r.isXaxis = BKP_TRUE;
		r.eq.a_ = -10000 ;
		return r;
	}
	
	r.isXaxis = BKP_FALSE;
	r.eq.a_ = r.colinear.y / r.colinear.x ;
	r.eq.b_ = pts1.y - (r.eq.a_ * pts1.x);

	return r;
}

/*--------------------------------------------------------------------------------*/
BKP_2Dline bkp_geometry_get2DlineFromColinear(BKP_Vec2 pt, BKP_Vec2 col)
{
	BKP_2Dline r;

	r.pt1 = pt;
	r.colinear.x = col.x;
	r.colinear.y = col.y;
	r.normal.x = -col.y;
	r.normal.y = col.x;

	if(0 == col.x)
	{
		r.pt2 = bkp_vec2(r.pt1.x, r.pt1.y + 1);
		r.isXaxis = BKP_TRUE;
		return r;
	}

	r.isXaxis = BKP_FALSE;
	r.eq.a_ = r.colinear.y / r.colinear.x ;
	r.eq.b_ = pt.y - (r.eq.a_ * pt.x);

	r.pt2.x = r.pt1.x + 1;
	r.pt2.y = r.pt2.x * r.eq.a_ + r.eq.b_;

	return r;
}

/*--------------------------------------------------------------------------------*/
BKP_2Dline bkp_geometry_get2DlineFromNormal(BKP_Vec2 pt, BKP_Vec2 col)
{
	BKP_2Dline r;

	r.pt1 = pt;
	r.normal.x = col.x;
	r.normal.y = col.y;
	r.colinear.x = -col.y;
	r.colinear.y = col.x;

	if(0 == col.y)
	{
		r.pt2 = bkp_vec2(r.pt1.x, r.pt1.y + 1);
		r.isXaxis = BKP_TRUE;
		return r;
	}

	r.isXaxis = BKP_FALSE;
	r.eq.a_ = r.colinear.y / r.colinear.x ;
	r.eq.b_ = pt.y - (r.eq.a_ * pt.x);

	r.pt2.x = r.pt1.x + 1;
	r.pt2.y = r.pt2.x * r.eq.a_ + r.eq.b_;

	return r;
}

/*--------------------------------------------------------------------------------*/
int bkp_geometry_getLineIntersection2(BKP_2Dline l1, BKP_2Dline l2, vec2_t *pts)
{
	if(BKP_TRUE == l1.isXaxis && BKP_TRUE == l2.isXaxis)
	{
		if(l1.pt1.x == l2.pt1.x)
			return -1;
		else
		{
			return 0 ;
		}
	}
	else if(BKP_FALSE == l1.isXaxis && BKP_FALSE == l2.isXaxis)
	{
		l1.eq.a_ = -l1.eq.a_;
		l1.eq.b_ = -l1.eq.b_;

		float xx = l1.eq.a_ + l2.eq.a_;
		float bb = -(l1.eq.b_ + l2.eq.b_);

		if(xx == 0)
		{
			if(bb == 0)
				return -1;
			return 0;
		}

		pts->x = bb / xx;
		pts->y = l2.eq.a_ * pts->x + l2.eq.b_;
	}
	else
	{
		BKP_2Dline * ver, *dro; //vertical et doite non verticale
		if(BKP_TRUE == l1.isXaxis) 
		{
			ver = &l1;
			dro = &l2;
		}
		else
		{
			ver = &l2; 
			dro = &l1;
		}

		pts->x = ver->pt1.x;
		pts->y = dro->eq.a_ * ver->pt1.x + dro->eq.b_;
	}

	return 1;
}


/*--------------------------------------------------------------------------------*/
int bkp_geometry_getSegmentsIntersection2(BKP_2Dline l1, BKP_2Dline l2, vec2_t *pts)
{

	int val = bkp_geometry_getLineIntersection2(l1, l2, pts);
	if(0 == val)
		return 0;


	if(val == -1)
	{
		float dx = fabs((l1.pt1.x + l1.pt2.x) - (l2.pt1.x + l2.pt2.x)) ;
		if( dx > fabs(l1.pt1.x - l1.pt2.x) + fabs(l2.pt1.x - l2.pt2.x) )
		{
			return 0;
		}
		float dy = fabs((l1.pt1.y + l1.pt2.y) - (l2.pt1.y + l2.pt2.y)) ;
		if( dy > fabs(l1.pt1.y - l1.pt2.y) + fabs(l2.pt1.y - l2.pt2.y) )
		{
			return 0;
		}

		vec2_t * q = &l1.pt1;	
		if(BKP_TRUE == l1.isXaxis)
		{
			if((l1.pt1.y > l2.pt1.y && l1.pt1.y > l2.pt2.y) || (l1.pt1.y < l2.pt1.y && l1.pt1.y < l2.pt2.y))
				q = &l1.pt2;
		}
		else
		{
			if((l1.pt1.x > l2.pt1.x && l1.pt1.x > l2.pt2.x) || (l1.pt1.x < l2.pt1.x && l1.pt1.x < l2.pt2.x))
				q = &l1.pt2;
		}
		pts->x = q->x;
		pts->y = q->y;
		return 1;
	}
	

	if((pts->x > l1.pt1.x && pts->x > l1.pt2.x) || (pts->x < l1.pt1.x && pts->x < l1.pt2.x) ||
		(pts->x > l2.pt1.x && pts->x > l2.pt2.x) || (pts->x < l2.pt1.x && pts->x < l2.pt2.x))
			return 0;
	if((pts->y > l1.pt1.y && pts->y > l1.pt2.y) || (pts->y < l1.pt1.y && pts->y < l1.pt2.y) ||
		(pts->y > l2.pt1.y && pts->y > l2.pt2.y) || (pts->y < l2.pt1.y && pts->y < l2.pt2.y))
			return 0;

	return 1;
}

/*--------------------------------------------------------------------------------*/
BKP_2Dline bkp_geometry_get2Dline(vec2_t pt1, vec2_t pt2)
{
	return (BKP_2Dline)  bkp_geometry_getAffineEq2(pt1,pt2);
}

/*--------------------------------------------------------------------------------*/
BKP_Vec2 bkp_geometry_get2DNormalVect(BKP_2Dline l)
{
	return (BKP_Vec2) bkp_vec2(0.0f, 0.0f);
}

/*--------------------------------------------------------------------------------*/
int bkp_geometry_segment2DinRectangle(BKP_Rect pts, BKP_Rect box)
{
	BKP_Vec2 * left = &pts.pt1, * right = &pts.pt2;
	BKP_Vec2 * up 	= &pts.pt1, * down 	= &pts.pt2;
	if(pts.pt2.x < pts.pt1.x) left 	= &pts.pt2, right = &pts.pt1;
	if(pts.pt2.y < pts.pt1.y) up	= &pts.pt2, up = &pts.pt1;

	if((left->x > box.x + box.w)||(right->x < box.x) || (down->y < box.y) || (up->y > box.y + box.h))
		return BKP_FALSE;

	return BKP_TRUE;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
void bkp_geometry_printLine(const char * info, BKP_2Dline l)
{
	if(BKP_TRUE == l.isXaxis)
		fprintf(stderr,"%s : X = %.2f\n",info, l.pt1.x);
	else
		fprintf(stderr,"%s : Y = %.2fX %c %.2f\n",info, l.eq.a_, l.eq.b_ > 0 ? '+': ' ', l.eq.b_);

	fprintf(stderr,"\t\tcolinear (%.2f,%.2f)\n",l.colinear.x, l.colinear.y);
	fprintf(stderr,"\t\tnormal   (%.2f,%.2f)\n",l.normal.x, l.normal.y);
	fprintf(stderr,"\t\tpts (%.2f,%.2f) - (%.2f,%.2f)\n",l.pt1.x, l.pt1.y,l.pt2.x, l.pt2.y);
	//fprintf(stderr,"\t\tcolinear (%.2f,%.2f) ; normal(%.2f,%.2f)\n",l.colinear.x, l.colinear.y, l.normal.x, l.normal.y);

	return;
}

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

