/* -*- linux-c -*-
 *  grav.h : groupe les fonctions en rapport avec les calculs gravitationnels
 *
 *  Copyright (C) 2001, 2002 Gwenhael LE MOINE, Michel SALVADORI
 *
 *   This file is part of OpenGLavity.
 *
 *   OpenGLavity is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   OpenGLavity is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with OpenGLavity; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * 18/10/2002 : - better twoWorldsCollide_2D, this one works ;-)
 *              - better calculus of nbBodies_aff, this one works ;-)
 *              - some clarification in functions and variables names
 *
 * 21/10/2002 : - exported absorbtion calculus in absorb_*D
 *              - created bounce_*D (do nothing yet...)
 */

#ifndef GRAV_H
#define GRAV_H

#include <math.h>

/*
le calcul se fait en 2 passes :
	1. calcul de l'accélération et modification de VX, VY, VZ
	2. modification des positions
*/

//	variables
static double G = 0.0000000000667f;

static int distanceMax = 100;

// les macros ça va plus vite que les fonctions :D
#define sq(n) ((n)*(n))

/*****************************************************************************/
/************************************** 2D ***********************************/
/*****************************************************************************/

#define computeAccBody_2D( body_1, body_2 ) {\
  double dx = body_2->X - body_1->X;\
  double dy = body_2->Y - body_1->Y;\
  double d2 = sq( dx ) + sq( dy );\
  double acceleration = G*( body_2->M/d2 ) / sqrt( d2 );\
  body_1->VX += dx * acceleration;\
  body_1->VY += dy * acceleration;\
/* DOIT-ON AUSSI MODIFIER body_2 ?!!!, a l'oeil ca a l'air tout aussi plausible !!*/\
  body_2->VX -= dx * acceleration; \
  body_2->VY -= dy * acceleration; \
}\

#define computePositions_2D( n, tb ) {\
  int i;\
  for ( i=n ; i-- ; ) {\
    tb[i].X += tb[i].VX;\
    tb[i].Y += tb[i].VY;\
  }\
}

#define destroyBody_2D( body ) {\
  set_body( 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, -1, body );\
}

#define absorb_2D( i, j ) {\
  tb[i].X = ( tb[j].M * tb[j].X + tb[i].M * tb[i].X ) / ( tb[j].M + tb[i].M );\
  tb[i].Y = ( tb[j].M * tb[j].Y + tb[i].M * tb[i].Y ) / ( tb[j].M + tb[i].M );\
  tb[i].M += tb[j].M;\
  tb[i].D = 2*pow( pow( tb[i].D/2.0f,3 ) + pow( tb[j].D/2.0f,3 ),0.333f );\
  tb[i].VX = ( tb[j].VX * tb[j].M + tb[i].VX * tb[i].M ) / ( tb[j].M + tb[i].M );\
  tb[i].VY = ( tb[j].VY * tb[j].M + tb[i].VY * tb[i].M ) / ( tb[j].M + tb[i].M );\
\
  destroyBody_2D( &( tb[j] ) );\
}

#define bounce_2D( i, j ) {\
  double coef = tb[i].Elasticity * tb[j].Elasticity; \
  double coef_temp; \
  /* on X */ \
  if ( abs(tb[i].X)>0 &&  abs(tb[j].X)>0 ) \
    if ( (tb[i].X / abs(tb[i].X)) !=  (tb[j].X / abs(tb[j].X)) ) \
      coef_temp = coef * -1; \
    else \
      coef_temp = coef; \
  tb[i].X *= coef_temp; \
  tb[j].X *= coef_temp; \
  /* on Y */ \
  if ( abs(tb[i].Y)>0 &&  abs(tb[j].Y)>0 ) \
    if ( (tb[i].Y / abs(tb[i].Y)) !=  (tb[j].Y / abs(tb[j].Y)) ) \
      coef_temp = coef * -1; \
    else \
      coef_temp = coef; \
  tb[i].Y *= coef_temp; \
  tb[j].Y *= coef_temp; \
}

#define twoWorldsCollide_2D( n, tb ) {\
  int i;\
  int j;\
  float d;\
  float dd;\
\
  for ( i=nbBodies ; i--; )\
    if ( tb[i].Color != -1 )\
      for ( j=i ;j--; )\
        if ( tb[j].Color != -1 )\
          if ( i != j ) {\
            d = sqrt( sq( tb[j].X - tb[i].X ) + sq( tb[j].Y - tb[i].Y ) );\
            dd = ( tb[i].D + tb[j].D ) ;\
            if ( dd >= d ) {\
              nbCollisions++;\
              if ( tb[i].M >= tb[j].M )\
                absorb_2D( i, j )\
              else\
                absorb_2D( j, i )\
            }\
          }\
}

#define lost_2D( n, tb ) {\
  int i;\
  for ( i=n ; i-- ; )\
    if ( tb[i].Color != -1 )\
      if ( ( tb[i].X > distanceMax ) || ( tb[i].Y > distanceMax ) ) {\
        nbLosts++;\
        destroyBody_2D( &( tb[i] ) );\
      }\
}

static void GE_iterate_2D( const int nbBodies, T_Body* tb ) {
  int i;
  int j;

#ifdef HAVE_GLUI
  nbBodies_aff = 0;
#endif
  for ( i=nbBodies ; i-- ; )
    if ( tb[i].Color != -1 ) {
#ifdef HAVE_GLUI
      nbBodies_aff++;
#endif
      for ( j=i ; j-- ; )
	if ( tb[j].Color != -1 )
	  computeAccBody_2D( ( &tb[i] ), ( &tb[j] ) )
	}

  computePositions_2D( nbBodies, tb )

  if ( grav_engine_switches.collisions )
    twoWorldsCollide_2D( nbBodies, tb )
  if ( grav_engine_switches.losts )
    lost_2D( nbBodies, tb )
}

/*****************************************************************************/
/************************************** 3D ***********************************/
/*****************************************************************************/

#define computeAccBody_3D( body_1, body_2 ) { \
  double dx = body_2->X - body_1->X; \
  double dy = body_2->Y - body_1->Y; \
  double dz = body_2->Z - body_1->Z; \
  double d2 = sq( dx ) + sq( dy ) + sq( dz ); \
  double acceleration = G*( body_2->M/d2 ) / sqrt( d2 ); \
  body_1->VX += dx * acceleration; \
  body_1->VY += dy * acceleration; \
  body_1->VZ += dz * acceleration; \
/* DOIT-ON AUSSI MODIFIER body_2 ?!!!, a l'oeil ca a l'air tout aussi plausible !!*/\
  body_2->VX -= dx * acceleration; \
  body_2->VY -= dy * acceleration; \
  body_2->VZ -= dz * acceleration; \
}

#define computePositions_3D( n, tb ) {\
  int i;\
  for ( i=n ; i-- ; ) {\
    tb[i].X += tb[i].VX;\
    tb[i].Y += tb[i].VY;\
    tb[i].Z += tb[i].VZ;\
  }\
}

#define destroyBody_3D( body ) {\
  set_body( 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, -1, body );\
}

#define absorb_3D( i, j ) {\
  tb[i].X = ( tb[j].M * tb[j].X + tb[i].M * tb[i].X ) / ( tb[j].M + tb[i].M );\
  tb[i].Y = ( tb[j].M * tb[j].Y + tb[i].M * tb[i].Y ) / ( tb[j].M + tb[i].M );\
  tb[i].Z = ( tb[j].M * tb[j].Z + tb[i].M * tb[i].Z ) / ( tb[j].M + tb[i].M );\
  tb[i].M += tb[j].M;\
  tb[i].D = 2*pow( pow( tb[i].D/2.0f,3 ) + pow( tb[j].D/2.0f,3 ),0.333f );\
  tb[i].VX = ( tb[j].VX * tb[j].M + tb[i].VX * tb[i].M ) / ( tb[j].M + tb[i].M );\
  tb[i].VY = ( tb[j].VY * tb[j].M + tb[i].VY * tb[i].M ) / ( tb[j].M + tb[i].M );\
  tb[i].VZ = ( tb[j].VZ * tb[j].M + tb[i].VZ * tb[i].M ) / ( tb[j].M + tb[i].M );\
\
  destroyBody_3D( &( tb[j] ) );\
}

#define bounce_3D( i, j ) {\
  double coef = tb[i].Elasticity * tb[j].Elasticity; \
  double coef_temp; \
  /* on X */ \
  if ( abs(tb[i].X)>0 &&  abs(tb[j].X)>0 ) \
    if ( (tb[i].X / abs(tb[i].X)) !=  (tb[j].X / abs(tb[j].X)) ) \
      coef_temp = coef * -1; \
    else \
      coef_temp = coef; \
  tb[i].X *= coef_temp; \
  tb[j].X *= coef_temp; \
  /* on Y */ \
  if ( abs(tb[i].Y)>0 &&  abs(tb[j].Y)>0 ) \
    if ( (tb[i].Y / abs(tb[i].Y)) !=  (tb[j].Y / abs(tb[j].Y)) ) \
      coef_temp = coef * -1; \
    else \
      coef_temp = coef; \
  tb[i].Y *= coef_temp; \
  tb[j].Y *= coef_temp; \
  /* on Z */ \
  if ( abs(tb[i].Z)>0 &&  abs(tb[j].Z)>0 ) \
    if ( (tb[i].Z / abs(tb[i].Z)) !=  (tb[j].Z / abs(tb[j].Z)) ) \
      coef_temp = coef * -1; \
    else \
      coef_temp = coef; \
  tb[i].Z *= coef_temp; \
  tb[j].Z *= coef_temp; \
}

#define twoWorldsCollide_3D( n, tb ) {\
  int i;\
  int j;\
  float d;\
  float dd;\
\
  for ( i=nbBodies ; i--; )\
    if ( tb[i].Color != -1 )\
      for ( j=i ;j--; )\
        if ( tb[j].Color != -1 )\
          if ( i != j ) {\
            d = sqrt( sq( tb[j].X - tb[i].X ) + sq( tb[j].Y - tb[i].Y ) + sq( tb[j].Z - tb[i].Z ) );\
            dd = ( tb[i].D + tb[j].D ) ;\
            if ( dd >= d ) {\
              nbCollisions++;\
              if ( tb[i].M >= tb[j].M )\
                absorb_3D( i, j )\
              else\
                absorb_3D( j, i )\
            }\
          }\
}

#define twoWorldsBounce_3D( n, tb ) {\
  int i;\
  int j;\
  float d;\
  float dd;\
\
  for ( i=nbBodies ; i--; )\
    if ( tb[i].Color != -1 )\
      for ( j=i ;j--; )\
        if ( tb[j].Color != -1 )\
          if ( i != j ) {\
            d = sqrt( sq( tb[j].X - tb[i].X ) + sq( tb[j].Y - tb[i].Y ) + sq( tb[j].Z - tb[i].Z ) );\
            dd = ( tb[i].D + tb[j].D ) ;\
            if ( dd >= d ) {\
              nbCollisions++;\
              if ( tb[i].M >= tb[j].M )\
                bounce_3D( i, j )\
              else\
                bounce_3D( j, i )\
            }\
          }\
}

#define lost_3D( n, tb ) {\
  int i;\
  for ( i=n ; i-- ; )\
    if ( tb[i].Color != -1 )\
      if ( ( tb[i].X > distanceMax ) || ( tb[i].Y > distanceMax ) || ( tb[i].Z > distanceMax ) ) {\
        nbLosts++;\
        destroyBody_3D( &( tb[i] ) );\
      }\
}

static void GE_iterate_3D( const int nbBodies, T_Body* tb ) {
  int i;
  int j;

#ifdef HAVE_GLUI
  nbBodies_aff = 0;
#endif
  for ( i=nbBodies ; i-- ; )
    if ( tb[i].Color != -1 ) {
#ifdef HAVE_GLUI
      nbBodies_aff++;
#endif
      for ( j=i ; j-- ; )
        if ( tb[j].Color != -1 )
	  computeAccBody_3D( ( &tb[i] ), ( &tb[j] ) )
    }

  if ( grav_engine_switches.collisions == BOUNCE)
    twoWorldsCollide_3D( nbBodies, tb )

  computePositions_3D( nbBodies, tb )

  if ( grav_engine_switches.collisions == ABSORB)
    twoWorldsCollide_3D( nbBodies, tb )
  if ( grav_engine_switches.losts )
    lost_3D( nbBodies, tb )
}

#endif
