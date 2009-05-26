/*
 *  main.cpp : la fonction main, les fonctions d'initialisation
 *             et les fonctions OpenGL
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

// include C
#include <time.h>
#include <getopt.h>

// OpenGLavity's includes
#include "config.h"

#include "globalvars.h"
#include "es.h"
#include "grav.h"
#include "glutstuff.h"
#include "gluistuff.h"

/*
 ! function GLUT call when it have to redraw the OpenGL window
 */
static void redraw(void) {
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glPushMatrix();
    ptr_movements(current_body);
    scene();
  glPopMatrix();
  glutSwapBuffers();
}

/*
 ! function GLUT call when it have to reshape the OpenGL window
 ! useful comment, isn't it ?! ;))
 */
static void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(65., (float)width / (float)height, 1.0, 500.0); 
  //glFrustum(-width/100, width/100, -height/100, height/100, 1, 500);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*
 ! function GLUT call when it have nothing else to do :
 !  here is where we compute FPS, refresh GLUI gui and
 !  finally asking to refresh main (OpenGL) window
 */
static void idle(void) {
//calcul FPS
  frame++;
  
  if (NB_ITER_NOGRAFX > 0)
    nbFramesDones++;
  
  temps=glutGet(GLUT_ELAPSED_TIME);
  
  if (temps - tempsbase > intervalle_fps) {
    fps = frame*1000.0/(temps-tempsbase);
    tempsbase = temps;
    frame = 0;
#ifdef HAVE_GLUI
    if(glui_controlWindow != NULL)
      glui_controlWindow->sync_live();
#endif
    total_f += fps;
    if ((deadline_bench != -1) && (temps/1000.0f >= deadline_bench))
      quitter(0);
	else
	  if ((NB_ITER_NOGRAFX != -1) && (NB_ITER_NOGRAFX >= nbFramesDones))
	    quitter(0);
  }
  //fin calcul FPS
  if (gui_switches.pause == OFF)
    ptr_GE_iterate(nbBodies, array_bodies);
  
#ifdef HAVE_GLUI
  if (glutGetWindow() != idF)
    glutSetWindow(idF);
#endif
  glutPostRedisplay();
}

static void init() {
  /* initializing gravity engine switches */
  grav_engine_switches.collisions = OFF_C;
  grav_engine_switches.losts      = OFF;

#ifdef HAVE_GLUT
  /* initializing gui switches */
  gui_switches.moving     = OFF;
  gui_switches.fullscreen = OFF;
  gui_switches.pause      = OFF;
  gui_switches.glui       = OFF;
  gui_switches.controls   = ON;
  gui_switches.graphics   = OFF;

  /* initializing functions pointers */
  ptr_translate = _3DTranslate;
  ptr_draw = drawSphere;
  ptr_movements = fixedCamera;
  ptr_GE_iterate = GE_iterate_3D;
#endif
}

#ifdef HAVE_GLUT
  /*
   ! initialize GLUT things that do not depend of GLUI presence or not
   */
  static void init_glut(void) {
    char title[25];

    glEnable(GL_DEPTH_TEST);
  
#ifdef LIGHT_0
    glEnable(GL_LIGHTING);
  
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Colors[5]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Colors[5]);
    glLightfv(GL_LIGHT0, GL_SHININESS, Colors[5]);
    glLightfv(GL_LIGHT0, GL_POSITION, pos_light0);
    glEnable(GL_LIGHT0);
#endif
  
    glEnable(GL_RGBA | GL_DEPTH | GL_DOUBLE);
  
    /* building a call-list for axes drawing */
    glNewList(LST_AXES, GL_COMPILE);
      glBegin(GL_LINES);
        glColor4fv(Colors[7]); // apparently OpenGL don't give a fuck about that ?! :/
	glVertex3f(AXES_SIZE, 0, 0);
	glVertex3f(-AXES_SIZE, 0, 0);
	glVertex3f(0, AXES_SIZE, 0);
	glVertex3f(0, -AXES_SIZE, 0);
	glVertex3f(0, 0, AXES_SIZE);
	glVertex3f(0, 0, -AXES_SIZE);
      glEnd();
    glEndList();

    glutInitWindowSize(largeur, hauteur);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    sprintf(title, "%s v%s.%s.%s %s",\
	    APP_NAME, MAJOR, MINOR, REVISION, LANGUAGE);
    idF = glutCreateWindow( title);
    glutSetWindow(idF);
    glutPositionWindow(4, 25);
    
    glutDisplayFunc(redraw);
    glutMotionFunc(motion);
    make_menu();
    if (gui_switches.fullscreen == ON)
      glutFullScreen();
  }
#endif /* HAVE_GLUT */

#ifdef HAVE_GLUI
  /*
   ! initialize GLUI things
   */
  static void init_glui(void) {
    GLUI_Master.set_glutIdleFunc(idle);
    
    GLUI_Master.set_glutReshapeFunc(reshape);
    
    GLUI_Master.set_glutKeyboardFunc(key);
    GLUI_Master.set_glutSpecialFunc(special);
    
    GLUI_Master.set_glutMouseFunc(mouse);
    make_controlWindow();
    
    glui_controlWindow->set_main_gfx_window(idF);
  }
#endif /* HAVE_GLUI */

/*
 ! parse commandline arguments using GNU's getopt_long()
 !  TODO : find a MS Windows equivalent for getopt_long()
 !         (and cross fingers so that MS die soon and those
 !          portability headaches disappears...)
 */
static int parse_args(int argc, char* argv[]) {
  int option_index;
  char c = '?';
  
  char* optstring = "vhuxFcl::n:i:o:t:";
  static struct option long_options[] =
    {
      /* options with no arguments */
      {"version",		no_argument,		NULL, 'v'},
      {"help",			no_argument,		NULL, 'h'},
      {"glui-interface",	no_argument,		NULL, 'u'},
      {"glut-interface",	no_argument,		NULL, 'x'},
      {"fullscreen",		no_argument,		NULL, 'F'},
      {"collisions",		no_argument,		NULL, 'c'},

      /* options with optional arguments */
      {"losts", 		optional_argument,	NULL, 'l'},

      /* options with required arguments */
      {"iterations",		required_argument,	NULL, 'n'},
      {"input-file",		required_argument,	NULL, 'i'},
      {"output-file",		required_argument,	NULL, 'o'},
      {"duration",		required_argument,	NULL, 't'},

      /* end of array */
      {0, 0, 0, 0}
    };
  
  char* help_text = "OpenGLavity [options]\n\
\t-v --version :\n\t\t affiche le numéro de version\n\
\t-h --help :\n\t\t ce que vous etes en train de lire\n\
\t-i<filename> --input-file=<filename> :\n\t\t file (.unv) à ouvrir (indispensable)\n\
\t-o<filename> --output-file=<filename> :\n\t\t file (.unv) à (éventuellement) écrire\n\
\t-t<seconds> --duration=<seconds> :\n\t\t fixer un temps d'éxécution limité en secondes (-x implicite)\n\
\t-n<number> --iterations=<number> :\n\t\t fixer un nombre fini d'itérations à calculer\n\
\t-x --glut-interface :\n\t\t lancer l'interface OpenGL\n\
\t-u --glui-interface :\n\t\t lancer l'interface GLUI (-x implicite)\n\
\t-c --collisions :\n\t\t active la gestion des collisions\n\
\t-l[distance] --losts[=distance] :\n\t\t active la gestion du controle de distance\n\
\t-F --fullscreen :\n\t\t lancer l'interface OpenGL en plein ecran (-x implicite)\n\n";

  if (argc == 1) {
    fprintf(stdout, "OpenGLavity : Les arguments sont manquants.\nPour en savoir davantage, faites: `OpenGLavity --help'.\n");
    exit(-1);
  }
  
  
  while(c != EOF) {
    c = getopt_long(argc, argv, optstring, long_options, &option_index);
    
    switch(c) {
    case 'v' :
      fprintf(stdout, "%s v%s.%s.%s %s\n\n", APP_NAME, MAJOR, MINOR, REVISION, LANGUAGE);
      exit(0);
      break;
    case 'h' :
      fprintf(stdout, "%s", help_text);
      exit(0);
      break;
    case 'i' :
      sprintf(file, "%s", optarg);
      if (load_universe( file ) != 0)
	quitter(-1);
      break;
    case 'o' :
      outfile = (char*)malloc(strlen(optarg)*sizeof(char));
      strcpy(outfile, optarg);
      break;
    case 't' :
      gui_switches.graphics = ON; /* -x implicite */
      NB_ITER_NOGRAFX = -1;
      deadline_bench = atoi(optarg);
      break;
    case 'n' :
      deadline_bench = -1;
      NB_ITER_NOGRAFX = atoi(optarg);
      break;
    case 'x' :
      gui_switches.graphics = ON;
      break;
    case 'F' :
      gui_switches.graphics = ON; /* -x implicite */
      gui_switches.fullscreen = ON;
      break;
    case 'c' :
      grav_engine_switches.collisions = BOUNCE;
#ifdef HAVE_GLUI
      var_collisions = grav_engine_switches.collisions;
#endif
      break;
    case 'l' :
      grav_engine_switches.losts = ON;
#ifdef HAVE_GLUI
      var_losts = grav_engine_switches.losts;
#endif
      if (optarg != 0)
	distanceMax = atoi(optarg);
      break;
    case 'u' :
      gui_switches.graphics = ON; /* -x implicite */
      gui_switches.glui = ON;
      break;
    case '?' :
    case ':' :
      quitter(0);
      break;
    default : break;
    }
  }
  
  if (optind < argc) {
    printf ("Arguments ne constituant pas des options : ");
    while (optind < argc)
      printf ("%s ", argv[optind++]);
    printf ("\n");
  }
  
  return(optind);
}


/*
 ! Guess what... this the main function ;-)
 ! TODO :
 !     - clean up that mess !
 */
int main(int argc, char* argv[]) {
  fprintf(stdout, "%s v%s.%s.%s %s\nCopyright (C) 2001, 2002 Gwenhael LE MOINE, Michel SALVADORI\n\n%s v%s.%s.%s %s est fourni sans AUCUNE GARANTIE; pour les\ndetails voir LICENSE. Ceci est un logiciel libre, et vous êtes libre de le\nredistribuer sous certaines conditions, voir COPYING pour les details.\n\n", APP_NAME, MAJOR, MINOR, REVISION, LANGUAGE, APP_NAME, MAJOR, MINOR, REVISION, LANGUAGE );

#ifdef HAVE_GLUI
  fprintf(stdout, "Using GLUI v%f\n", GLUI_Master.get_version());
#endif

  init();

  if ( parse_args(argc, argv) < 1 )
    exit(-1);

  if ( gui_switches.graphics == OFF ) {
///////////////////         NO GRAPHICS, JUST CLI         /////////////////////
    char* date;
    time_t date_debut;
    time_t date_fin;

    time( &date_debut );
    date = asctime(localtime( &date_debut ));

    fprintf(stdout, "Et c'est parti pour %i iterations\ndate debut : %s\n", NB_ITER_NOGRAFX, date);

    init();
    if ((NB_ITER_NOGRAFX > 0) && (gui_switches.graphics == OFF)) {
      int z;
      for(z=NB_ITER_NOGRAFX ; z-- ;)
	ptr_GE_iterate(nbBodies, array_bodies);
    } else {
      fprintf(stdout, "paramètre manquant : -n<nb_iterations>\n");
      quitter(-1);
    }

    time( &date_fin );
    date = asctime(localtime( &date_fin ));
    fprintf(stdout, "date fin : %s\n", date);
    fprintf(stdout, "duree en secondes : %18.0f\n", difftime(date_fin, date_debut));
    save_universe(outfile, array_bodies);
  }
  else {
////////////////             OPENGL INTERFACE//////////////////////////////////
    init_glut();
#ifdef HAVE_GLUI
    switch (gui_switches.glui) {
    case OFF :
#endif
      glutIdleFunc(idle);
      glutReshapeFunc(reshape);

      glutKeyboardFunc(key);
      glutSpecialFunc(special);

      glutMouseFunc(mouse);
#ifdef HAVE_GLUI
      break;
    case ON :
      init_glui();
      break;
    default : exit(-1);
      break;
    }
#endif

//    init();

    glutMainLoop();
  }
  quitter(0);
}

