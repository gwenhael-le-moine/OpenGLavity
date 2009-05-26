/*
 *  glutstuff.h : groupe ce qui gère l'affichage dépendant
 *                de la bibliothèque GLUT
 *
 *  Copyright 2001, 2002 Gwenhael LE MOINE, Michel SALVADORI
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

/*
 * 21/10/2002 : fixed bug : switching collisions &| lost modify glui gui
 */

#ifndef GLUTSTUFF_H
#define GLUTSTUFF_H

#include <GL/glut.h>
#include <GL/glu.h>

int current_body = 0;

#ifdef SEPARATE_CONTROLS
  #define largeur 600
#else
  #ifdef HAVE_GLUI
    #define largeur 966
  #else
    #define largeur 600
  #endif
#endif

#define hauteur 600

#define AXES_SIZE 1000
#define DISCRETISATION 10

//	variables
#ifdef HAVE_GLUI
  #include <GL/glui.h> 

  static GLUI* glui_controlWindow;
  static int var_fullscreen = gui_switches.fullscreen;
  static int var_collisions = grav_engine_switches.collisions;
  static int var_losts = grav_engine_switches.losts;
#endif

static GLfloat view_rotx = 0.0 ;
static GLfloat view_roty = 0.0 ;
static GLfloat view_rotz = 0.0 ;

// save window parameters
int window_x      = 1;
int window_y      = 1;
int window_width  = 600;
int window_height = 600;

// mouse stuff
static int startx;
static int starty;

// 
static float distance = 0.0f;
static float transl[2] = {0.0f, 0.0f};
static float rotarray[16] = { 1.0, 0.0, 0.0, 0.0,
			      0.0, 1.0, 0.0, 0.0,
			      0.0, 0.0, 1.0, 0.0,
			      0.0, 0.0, 0.0, 1.0 };

static char file[1024];
static char* outfile = "saved.unv";   //      file dans lequel on vas écrire

static int intervalle_fps = 500; // intervalle en ms entre les calculs de fps

static float total_f = 0; // gestion FPS

//couleurs (à mettre dans un file ?, parser rgb.txt ?)
static float Colors[][4] = {
  {1.0f,   0.0f,   0.0f,   0.0f},		//0: rouge
  {0.0f,   1.0f,   0.0f,   0.0f},		//1: vert
  {0.0f,   0.0f,   1.0f,   0.0f},		//2: bleu
  {1.0f,   1.0f,   0.0f,   0.0f},		//3: jaune
  {0.725f, 0.725f, 0.725f, 0.0f},		//4: gris
  {1.0f,   1.0f,   1.0f,   0.0f},		//5: blanc
  {0.0f,   0.0f,   0.0f,   0.0f},		//6: noir
  {1.0f,   0.0f,   1.0f,   0.0f},		//7: rose
};

//	fonctions
inline void quitter( int code_out ) {
    free(array_bodies);
    float uptime = (glutGet(GLUT_ELAPSED_TIME)/1000.0f);
    float ftmp = total_f / uptime;

    if (uptime > 0.0f)
	fprintf(stdout, "durée d'execution : %10.3f seconde(s)\nFPS moyen : %10.10f\n", uptime, ftmp/(1000.0f/intervalle_fps));
    exit(code_out);
}

inline void rotating( void ) {
  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);

  glMultMatrixf(rotarray);
}

inline void center_view( void ) {
  int i;
  float x_tot=0;
  float y_tot=0;

  for ( i=nbBodies ; i-- ; ) {
    x_tot+=array_bodies[i].X;
    y_tot+=array_bodies[i].Y;
  }

  transl[0] = x_tot/nbBodies;
  transl[1] = y_tot/nbBodies;
}

static void fixedCamera(int whichOne) {
#ifdef SEPARATE_CONTROLS
  glTranslatef(transl[0], transl[1], -34.0f-distance);
#else
  #ifdef HAVE_GLUI
    glTranslatef(transl[0] - 13.0f, transl[1], -34.0f-distance);
  #else
    glTranslatef(transl[0], transl[1], -34.0f-distance);
  #endif
#endif
  rotating();
}

static void embarquedCamera(int whichOne) {
  rotating();
  glTranslatef(array_bodies[whichOne].X, array_bodies[whichOne].Y, array_bodies[whichOne].Z);
}

static void _2DTranslate(const float x, const float y, const float z) {
  glTranslatef(x, y, 0.0);
}

static void _3DTranslate(const float x, const float y, const float z) {
  glTranslatef(x, y, z);
}

static void drawSphere(const double d) {
  glutSolidSphere(d, DISCRETISATION, DISCRETISATION);
}

static void drawPoint(const double d) {
  glBegin(GL_QUADS);
  glVertex3f(0.0f, -d  , 0.0f);
  glVertex3f(0.0f, 0.0f, -d);
  glVertex3f(0.0f, d   , 0.0f);
  glVertex3f(0.0f, 0.0f, d);

  glVertex3f(-d  , 0.0f, 0.0f);
  glVertex3f(0.0f, -d  , 0.0f);
  glVertex3f(d   , 0.0f, 0.0f);
  glVertex3f(0.0f, d   , 0.0f);
  glEnd();
}

inline void scene( void ) {
  int i;
  glCallList(LST_AXES);

  for (i=nbBodies ; i-- ;) {
    if (array_bodies[i].Color != -1) {
      glColor4fv(Colors[array_bodies[i].Color]);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, Colors[array_bodies[i].Color]);
      glMaterialfv(GL_FRONT, GL_SPECULAR, Colors[5]);
      glMaterialfv(GL_FRONT, GL_AMBIENT, Colors[5]);
      glMaterialf(GL_FRONT, GL_SHININESS, 128.0);
      glPushMatrix();
        ptr_translate(array_bodies[i].X, array_bodies[i].Y, array_bodies[i].Z);
        ptr_draw(array_bodies[i].D);
      glPopMatrix();
    }
  }
}

// Menu Glut
static void selectMenu(int selection) {
  switch (selection) {
  case -1 :
    break;
  case 1 :
    grav_engine_switches.collisions = (grav_engine_switches.collisions == BOUNCE) ? OFF_C : BOUNCE;
#ifdef HAVE_GLUI
    var_collisions = grav_engine_switches.collisions;
#endif
    break ;
  case 2 :
    gui_switches.pause = (gui_switches.pause == ON) ? OFF : ON;
    break ;
  case 3 :
    if (ptr_translate == _2DTranslate ) {
      ptr_translate = _3DTranslate;
      ptr_GE_iterate = GE_iterate_3D;
    }
    else {
      ptr_translate = _2DTranslate;
      ptr_GE_iterate = GE_iterate_2D;
    }
    break;
  case 4 :
    ptr_draw = (ptr_draw == drawSphere) ? drawPoint : drawSphere;
    break;
  case 5 :
    if (load_universe( file ) != 0)
      quitter(-1);
    break;
  case 7 :
    gui_switches.moving = OFF;
    view_rotx = view_roty = view_rotz =\
      distance =\
      transl[0] = transl[1] =\
      rotarray[1] = rotarray[2] = rotarray[3] = rotarray[4] =\
      rotarray[6] = rotarray[7] = rotarray[8] = rotarray[9] =\
      rotarray[11] = rotarray[12] = rotarray[13] = rotarray[14] = 0.0 ;
    rotarray[0] = rotarray[5] = rotarray[10] = rotarray[15] = 1.0;
    break;
  case 8 :
    ptr_movements = (ptr_movements == fixedCamera) ? embarquedCamera : fixedCamera;
    break;
  case 9 :
    grav_engine_switches.losts = (grav_engine_switches.losts == ON) ? OFF : ON;
#ifdef HAVE_GLUI
    var_losts = grav_engine_switches.losts;
#endif
    break ;
  case 10 :
    save_universe(outfile, array_bodies);
    break;
  case 11 :
    center_view();
    break;
  case 12 :
    if (gui_switches.fullscreen == OFF) {
      gui_switches.fullscreen = ON;
#ifdef HAVE_GLUI
      var_fullscreen = gui_switches.fullscreen;
#endif
      // saving window size and position
      window_x = glutGet(GLUT_WINDOW_X);
      window_y = glutGet(GLUT_WINDOW_Y);
      window_width = glutGet(GLUT_WINDOW_WIDTH);
      window_height = glutGet(GLUT_WINDOW_HEIGHT);

      // going fullscreen
      glutFullScreen();
    }
    else {
      // back to windowed mode with saved parameters
      glutReshapeWindow(window_width, window_height);
      glutPositionWindow(window_x, window_y);

      fprintf(stderr, "X: %i ; Y: %i ; H: %i ; W: %i\n", 
	      glutGet(GLUT_WINDOW_X),
	      glutGet(GLUT_WINDOW_Y),
	      glutGet(GLUT_WINDOW_HEIGHT),
	      glutGet(GLUT_WINDOW_WIDTH));


      gui_switches.fullscreen = OFF;
#ifdef HAVE_GLUI
      var_fullscreen = gui_switches.fullscreen;
#endif
    }
    break;
#ifdef HAVE_GLUI
  case 13 :
    if (gui_switches.glui == ON)
      if (gui_switches.controls == ON) {
	gui_switches.controls = OFF;
	glui_controlWindow->hide();
      }
      else {
	gui_switches.controls = ON;
	glui_controlWindow->show();
      }
    break;
#endif
  case 0 :
    quitter(0);
    break;
  default : break;
  }
  glutPostRedisplay();
}

static void make_menu(void) {
  glutCreateMenu(selectMenu);
#ifdef HAVE_GLUI
  if (gui_switches.glui == ON) {
    glutAddMenuEntry("<a>   Afficher/Cacher la fenetre de controle",13);
    glutAddMenuEntry(" ",-1);
  }
#endif
  glutAddMenuEntry("<c>   Activer/Désactiver la gestion des collisions",1);
  glutAddMenuEntry("<d>   Activer/Désactiver le controle de distance",9);
  glutAddMenuEntry("<p>   Pause ON/OFF",2);
  glutAddMenuEntry("<2,3> Activer/Désactiver la gestion de la 3D",3);
  glutAddMenuEntry("<s>   Point / Sphere",4);
  glutAddMenuEntry("<r>   Recharger le file",5);
  glutAddMenuEntry("<i>   Position initiale",7);
  glutAddMenuEntry("<v>   Centrer sur le systeme",11);
  glutAddMenuEntry("<f>   switch fullscreen",12);
  glutAddMenuEntry("<e>   Position camera",8);
  glutAddMenuEntry("<o>   Sauver dans saved.unv",10);
  glutAddMenuEntry(" ",-1);
  glutAddMenuEntry("<Esc> Quitter",0);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//	Souris
static void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      gui_switches.moving = ON;
      startx = x;
      starty = y;
    }
    if (state == GLUT_UP)
      gui_switches.moving = OFF;
  }
  if (button == GLUT_MIDDLE_BUTTON) {
    if (state == GLUT_DOWN) {}
    if (state == GLUT_UP) {}
  }
}

static void motion(int x, int y)
{
  if (gui_switches.moving == ON) {
    view_roty = view_roty + (x - startx);
    view_rotx = view_rotx + (y - starty);
    startx = x;
    starty = y;
    glutPostRedisplay();
  }
}

//	Clavier
//gestion des touches "normales"
static void key(unsigned char key,int x,int y) {
  switch ( key ) {
  case 'a' :
  case 'A' : selectMenu(13);
    break;
  case 'c' :
  case 'C' : selectMenu(1);
    break;
  case 'd' :
  case 'D' : selectMenu(9);
    break;
  case 'e' :
  case 'E' : selectMenu(8);
    break;
  case 'f' :
  case 'F' : selectMenu(12);
    break;
  case 'i' :
  case 'I' : selectMenu(7);
    break;
  case 'p' :
  case 'P' : selectMenu(2);
    break;
  case 'o' :
  case 'O' : selectMenu(10);
    break;
  case 'r' :
  case 'R' : selectMenu(5);
    break;
  case 's' :
  case 'S' : selectMenu(4);
    break;
  case 'v' :
  case 'V' : selectMenu(11);
    break;
  case 'z' : view_rotz += 2.0;
    break;
  case 'Z' : view_rotz -= 2.0;
    break;
  case '3' :
  case '2' : selectMenu(3);
    break;
  case '+' :
    if (current_body < nbBodies)
      do {
	current_body++;
      } while (array_bodies[current_body].Color == -1);
    fprintf(stdout, "current_body : %i\n", current_body);
    break;
  case '-' :
    if (current_body > 0)
      do {
	current_body--;
      } while (array_bodies[current_body].Color == -1);
    fprintf(stdout, "current_body : %i\n", current_body);
    break;
  case 'q' :
  case 'Q' :
  case '\033' : selectMenu(0);
    break ;
  }
}

//gestion des touches "spéciales"
static void special(int k, int x, int y) {
  switch (k) {
  case GLUT_KEY_PAGE_UP    : distance += 1.0;
    break;
  case GLUT_KEY_PAGE_DOWN  : distance -= 1.0;
    break;
  case GLUT_KEY_UP    : view_rotx += 2.0;
    break;
  case GLUT_KEY_DOWN  : view_rotx -= 2.0;
    break;
  case GLUT_KEY_LEFT  : view_roty += 2.0;
    break;
  case GLUT_KEY_RIGHT : view_roty -= 2.0;
    break;
  }
  glutPostRedisplay();
}

#endif
