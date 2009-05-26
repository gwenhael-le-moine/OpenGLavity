/*
 *  globalvars.h : regroupe certaines variables globales
 *                 et déclarations de type
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
 */

#ifndef GLOBALVARS_H
#define GLOBALVARS_H

enum T_switch {OFF, ON};
enum T_switch_collision {OFF_C, ABSORB, BOUNCE};

struct {
  T_switch_collision collisions;//= OFF;
  T_switch losts;     //= OFF;
} grav_engine_switches;

struct {
  T_switch moving;    //= OFF;
  T_switch fullscreen;//= OFF;
  T_switch pause;     //= OFF;
  T_switch glui;      //= ON;
  T_switch controls;  //= ON;
  T_switch graphics;  //= ON;
} gui_switches;

struct T_Body {
  double X; // position on X's axis
  double Y; // position on Y's axis
  double Z; // position on Z's axis
  
  double VX; // celerity on X's axis
  double VY; // celerity on Y's axis
  double VZ; // celerity on Z's axis
  
  double M; // Mass
  double D; // Diameter (all bodies are spherical)

  double Elasticity; // Elasticity -> (soft)0..1(hard)
  
  int    Color; // Color
};

//variables
// Système
static T_Body* array_bodies;

static int nbBodies = 2;		//	nombre de body
static int nbCollisions = 0;	//	nombre de collisions
static int nbLosts = 0;
// fin Système

static int NB_ITER_NOGRAFX = -1;
static int nbFramesDones = 0;

static int deadline_bench = -1;

//positions
#ifdef LIGHT_0
  static float pos_light0[] = {0.0f, 0.0f, 5.0f, 0.0f};
#endif

//gestion FPS
static int frame=0;
static int temps;
static int tempsbase=0;
//fin gestion FPS

// Call listes
#define LST_AXES 1
// fin Call listes

// pointeurs de fonctions
  // 2D / 3D
  static void (*ptr_GE_iterate)    (const int, T_Body*);

  // pointeur sur la fonction de translation avant draw (2D / 3D)
  static void (*ptr_translate)  (const float, const float, const float);

  // pointeur sur la fonction de draw (sphère / point)
  static void (*ptr_draw)     (const double);

  // caméra fixe / embarqué
  static void (*ptr_movements) (int);
// fin pointeurs de fonctions

#endif
