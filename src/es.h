/*
 *  es.h : gère les entrées/sorties
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

#ifndef ES_H
#define ES_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

int nbBodies_aff;

//	fonctions
//	fonction d'initialisation
static void set_body(double X,
		      double Y,
		      double Z,
		      double M,
		      double D,
		      double VX,
		      double VY,
		      double VZ,
		      int Color,
		      T_Body* body)
{
  body->X = X;
  body->Y = Y;
  body->Z = Z;
  body->M = M;
  body->D = D;
  body->VX = VX;
  body->VY = VY;
  body->VZ = VZ;

  body->Elasticity = 0.5;
  
  body->Color = Color;
}

//lit le file
static int load_universe(char* filename) {
  int i;
  int err;
  int color = 0;
  float tbP[8];
  FILE *fp;

  fp = fopen(filename, "r");
  if ( fp == NULL) {
    fprintf(stderr, "Error opening file %s.\n", filename);
    return(-1);
  }

  err = fscanf(fp, "%i\n", &nbBodies);
  if (err != 1)
    fprintf(stderr, "Erreur dans le nombre de paramètres lus\n\
\tFound only %i\n", err);

  if (err == EOF)
    fprintf(stderr, "Erreur, fin de file non attendue");

  array_bodies = (T_Body*) malloc(nbBodies * sizeof(T_Body));
  nbBodies_aff = nbBodies;
  
  for (i=0 ; i<nbBodies ; i++) {
    err = fscanf(fp, "%f %f %f %f %f %f %f %f %i\n",
		 &(tbP[0]), &(tbP[1]), &(tbP[2]), &(tbP[3]),
		 &(tbP[4]), &(tbP[5]), &(tbP[6]), &(tbP[7]),
		 &color);
    if (err != 9)
      fprintf(stderr, "Erreur dans le nombre de paramètres lus\n\tFound only %i\n", err);
    if (err == EOF)
      fprintf(stderr, "Erreur, fin de file non attendue");
    set_body(tbP[0], tbP[1], tbP[2], tbP[3],
	      tbP[4], tbP[5], tbP[6], tbP[7], color, &(array_bodies[i]));
  }
  fclose(fp);
  nbCollisions = 0;
  nbLosts = 0;
  return(0);
}

//écrit le file de paramètres
static int save_universe(char* filename, T_Body* tb) {
  int i;
  FILE *fp;

  if ( (fp = fopen(filename, "w+")) == NULL) {
    fprintf(stderr, "Error opening file %s.\n", filename);
    return(-1);
  }

  //données prérequises dans tous les modes
  fprintf(fp, "%i\n", nbBodies);

  //fin prérequis
  for (i=0 ; i<nbBodies ; i++)
    fprintf(fp, "%f %f %f %f %f %f %f %f %i\n",
	    tb[i].X, tb[i].Y, tb[i].Z, tb[i].M, tb[i].D, tb[i].VX, tb[i].VY, tb[i].VZ,
	    tb[i].Color);

  fclose(fp);

  return(0);
}

static void reload_file(char* file) {
  int err;
  err = load_universe(file);
  if (err < 0) {
    fprintf(stderr, "Erreur relecture file: %s", file);
    exit(-1);
  }
}

#endif
