/*
 *  gluistuff.h : groupe ce qui gère l'affichage dépendant de la bibliothèque GLUI
 *                                                ( http://www.cs.unc.edu/~rademach/glui )
 *
 *  Copyright ( C ) 2001, 2002 Gwenhael LE MOINE, Michel SALVADORI
 *
 *   This file is part of OpenGLavity.
 *
 *   OpenGLavity is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   ( at your option ) any later version.
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
 * 21/10/2002 : - file_index correspond to the current file if this one is in UNVS
 */


#ifndef GLUISTUFF_H
#define GLUISTUFF_H

#include <libgen.h>

static float fps = 0.0f; // gestion FPS

static int idF;	//	window ID

#ifdef HAVE_GLUI
  static GLUI_Listbox*    listget;
  static GLUI_EditText*   get;
  static GLUI_EditText*   get_outfile;
  static GLUI_Spinner*    aff_current_body;
//  static GLUI_EditText*    aff_current_body;
  static GLUI_Panel*      p_boutons;
  static GLUI_Panel*      sp_infos;
  static GLUI_Panel*      sp_rb_reglages;
  static GLUI_Panel*      rb_2d3d;
  static GLUI_Panel*      rb_collisions;
  static GLUI_Panel*      rb_spherepoint;
  static GLUI_Panel*      rb_camera;
  static GLUI_Panel*      p_change_body;
  static GLUI_Rollout*    ro_parametres;
  static GLUI_Rollout*    ro_movements;
//  static GLUI_Rollout*    ro_files;
  static GLUI_RadioGroup* rg_2d3d;
  static GLUI_RadioGroup* rg_collisions;
  static GLUI_RadioGroup* rg_spherepoint;
  static GLUI_RadioGroup* rg_camera;

  static int dummy_var2 = 0;
  static int dummy_var3 = 0;
  static int dummy_var4 = 0;
  static int dummy_var5 = 0;

  static int file_index = 0;

  static char** unvlist;

/*
 * Pour la liste déroulante de la gui
 * code tiré des articles programmation POSIX de Login, auteur : Gilbert Fernandes
 */
#define TAILLE_B 256

static void panique(void) {
  fprintf(stderr, "Not enough memory !\n");
  abort();
}

static char* getCurrentDir(void) {
  int taille = TAILLE_B;
  char* ptr;

  while(1) {
    ptr = (char*)malloc(taille);

    if (ptr == NULL)
      panique();

    if(getcwd(ptr, taille-1) != NULL)
      return(ptr);

    if (errno != ERANGE)
      panique();

    free(ptr);

    taille += TAILLE_B;
  }
}

static int build_unvlist(void) {
  struct dirent** namelist;
  int n, nb, nbr=0;
  int total_size = 0;

  fprintf(stdout, "current dir : %s\n", getCurrentDir());

  nb = n = scandir(UNVS, &namelist, 0, alphasort);
  if (nb < 0) {
    perror("scandir");
    return(-1);
  }
  else {
    while(n--) {
      if(strcasecmp((namelist[n]->d_name)+sizeof(namelist[n]->d_name)-4, ".unv") != 0)
	total_size += sizeof(namelist[n]->d_name);
    }
    if (unvlist != NULL)
      free(unvlist);
    unvlist = (char**)malloc(total_size*sizeof(char));
    n = nb;
    while(n--) {
      if(strcasecmp((namelist[n]->d_name)+sizeof(namelist[n]->d_name)-4, ".unv") != 0) {
	unvlist[n] = namelist[n]->d_name;
	nbr++;
      }
    }
    free (namelist);

    return(nbr);
  }
  exit(0);
}

  static void glui_react( int RAF ) {
    switch( RAF ) {
    case 99 :	sprintf( file, "%s", get->get_text());
      if (load_universe( file ) != 0)
	quitter(-1);
      break;
    case 98 :	selectMenu( 1 );	//	collisions ON/OFF
      break;
    case 97 :	selectMenu( 3 );	//	2D <=> 3D
      break;
    case 96 :	selectMenu( 4 );	//	sphere <=> point
      break;
    case 95 :	G = 0.0000000000667f;	//	reinit G
      break;
    case 94 :	selectMenu( 8 );	//	sphere <=> point
      break;
    case 93 :	selectMenu( 9 );	//	contrôle des distances ON/OFF
      break;
    case 92 :	save_universe( (char *)get_outfile->get_text(), array_bodies );
      break;
    case 91 :	sprintf( file, "%s%s", UNVS, unvlist[listget->get_int_val()] );
      if (load_universe( file ) != 0)
	quitter(-1);
      break;
    case 90 :       selectMenu( 13 ); // fenetre controles ON/OFF
      break;
    case 89 :       selectMenu( 12 ); // fullscreen ON/OFF
      break;
    case 88 :       key('-', 0, 0);
      break;
    case 87 :       key('+', 0, 0);
      break;
    default : break;
    }
  }

  static void make_controlWindow( void ) {
    int i, nb_files;
    char* curfile;

#ifdef SEPARATE_CONTROLS    
    glui_controlWindow = GLUI_Master.create_glui( "Controles", 0, largeur+15, 10 );
#else
    glui_controlWindow = GLUI_Master.create_glui_subwindow( idF, GLUI_SUBWINDOW_RIGHT );
#endif

    ro_movements = glui_controlWindow->add_rollout( "Mouvements", true );
      glui_controlWindow->add_rotation_to_panel( ro_movements, "Rotation", rotarray );
      glui_controlWindow->add_button_to_panel( ro_movements, "Position initiale", 7, selectMenu );
      glui_controlWindow->add_column_to_panel( ro_movements );
      glui_controlWindow->add_translation_to_panel( ro_movements, "Distance", GLUI_TRANSLATION_Z, &distance );
      glui_controlWindow->add_checkbox_to_panel( ro_movements, "Fullscreen", &var_fullscreen, 89, glui_react );
      glui_controlWindow->add_column_to_panel( ro_movements );
      glui_controlWindow->add_translation_to_panel( ro_movements, "Cadrage", GLUI_TRANSLATION_XY, transl );
      glui_controlWindow->add_button_to_panel( ro_movements, "Centrer la vue", 11, selectMenu );

    ro_parametres = glui_controlWindow->add_rollout( "Parametres", true );
      sp_infos = glui_controlWindow->add_panel_to_panel( ro_parametres, "", GLUI_PANEL_NONE );
        (glui_controlWindow->add_edittext_to_panel( sp_infos, "FPS",	GLUI_EDITTEXT_FLOAT, &fps ))->disable();
	(glui_controlWindow->add_edittext_to_panel( sp_infos, "nb body", GLUI_EDITTEXT_INT, &nbBodies_aff  ))->disable();
	glui_controlWindow->add_checkbox_to_panel( sp_infos, "Collisions", &var_collisions, 98, glui_react );
	glui_controlWindow->add_column_to_panel( sp_infos );
	(glui_controlWindow->add_edittext_to_panel( sp_infos, "nb collisions", GLUI_EDITTEXT_INT, &nbCollisions  ))->disable();
	( glui_controlWindow->add_edittext_to_panel( sp_infos, "nb perdu", GLUI_EDITTEXT_INT, &nbLosts  ))->disable();
	glui_controlWindow->add_checkbox_to_panel( sp_infos, "Controle distance", &var_losts, 93, glui_react );
      sp_rb_reglages = glui_controlWindow->add_panel_to_panel( ro_parametres, "", GLUI_PANEL_NONE );
        rb_2d3d = glui_controlWindow->add_panel_to_panel( sp_rb_reglages, "Rendu" );
	  rg_2d3d = glui_controlWindow->add_radiogroup_to_panel( rb_2d3d, &dummy_var3, 97, glui_react );
	    glui_controlWindow->add_radiobutton_to_group( rg_2d3d, "3D" );
	    glui_controlWindow->add_radiobutton_to_group( rg_2d3d, "2D" );
        rb_collisions = glui_controlWindow->add_panel_to_panel( sp_rb_reglages, "Rendu" );
	  rg_collisions = glui_controlWindow->add_radiogroup_to_panel( rb_collisions, &dummy_var5, 86, glui_react );
	    glui_controlWindow->add_radiobutton_to_group( rg_collisions, "OFF" );
	    glui_controlWindow->add_radiobutton_to_group( rg_collisions, "ABSORB" );
	    glui_controlWindow->add_radiobutton_to_group( rg_collisions, "BOUNCE" );
        glui_controlWindow->add_column_to_panel( sp_rb_reglages );
	rb_spherepoint = glui_controlWindow->add_panel_to_panel( sp_rb_reglages, "Dessin" );
	  rg_spherepoint = glui_controlWindow->add_radiogroup_to_panel( rb_spherepoint, &dummy_var2, 96, glui_react );
	  glui_controlWindow->add_radiobutton_to_group( rg_spherepoint, "Sphere" );
	  glui_controlWindow->add_radiobutton_to_group( rg_spherepoint, "Point" );
	glui_controlWindow->add_column_to_panel( sp_rb_reglages );
	rb_camera = glui_controlWindow->add_panel_to_panel( sp_rb_reglages, "Camera" );
	  rg_camera = glui_controlWindow->add_radiogroup_to_panel( rb_camera, &dummy_var4, 94, glui_react );
	    glui_controlWindow->add_radiobutton_to_group( rg_camera, "Fixe" );
	    glui_controlWindow->add_radiobutton_to_group( rg_camera, "Embarque" );
	  p_change_body = glui_controlWindow->add_panel_to_panel( rb_camera, "" );
//	    (glui_controlWindow->add_button_to_panel( p_change_body, "<", 88, glui_react ))->set_w(8);
//	    glui_controlWindow->add_column_to_panel( p_change_body );
//	    aff_current_body = glui_controlWindow->add_edittext_to_panel( p_change_body, "", GLUI_EDITTEXT_INT, &current_body  );
	    aff_current_body = glui_controlWindow->add_spinner_to_panel( p_change_body, "", GLUI_SPINNER_INT, &current_body  );
//	    aff_current_body->disable();
	    aff_current_body->set_w(4);
//	    glui_controlWindow->add_column_to_panel( p_change_body );
//	    (glui_controlWindow->add_button_to_panel( p_change_body, ">", 87, glui_react ))->set_w(8);

/* ro_files = glui_controlWindow->add_rollout( "Fichier", true ); */
/* listget = glui_controlWindow->add_listbox_to_panel( ro_files, UNVS, &file_index, 91, glui_react ); */
/* nb_files = build_unvlist(); */
/* for ( i=0 ; i<nb_files ; i++ ) */
/* if ((( unvlist[i] )[0] != '.' ) && ( strcasecmp(( unvlist[i] )+sizeof( unvlist[i] )-4, ".unv" ) != 0 )) */
/* listget->add_item( i, unvlist[i] ); */
/* curfile = basename(file); */
/* for ( i=0 ; i<nb_files ; i++) ; */
/* if ((( unvlist[i] )[0] != '.' ) && strcasecmp( curfile, unvlist[i]) == 0) { */
/* file_index=i; */
/* //	break; */
/* } */
/* get = glui_controlWindow->add_edittext_to_panel( ro_files, "Fichier", GLUI_EDITTEXT_TEXT ); */
/* get->set_text( file ); */
/* get->set_w( 280 ); */
/* glui_controlWindow->add_button_to_panel( ro_files, "Charger", 99, glui_react ); */
/* get_outfile = glui_controlWindow->add_edittext_to_panel( ro_files, "Fichier", GLUI_EDITTEXT_TEXT ); */
/* get_outfile->set_text( outfile ); */
/* get_outfile->set_w( 280 ); */
/* glui_controlWindow->add_button_to_panel( ro_files, "Sauver", 92, glui_react ); */
	    
	    p_boutons = glui_controlWindow->add_panel( "", GLUI_PANEL_NONE );
      glui_controlWindow->add_button_to_panel( p_boutons, "Pause", 2, selectMenu );
      glui_controlWindow->add_column_to_panel( p_boutons );
      glui_controlWindow->add_button_to_panel( p_boutons, "Fermer Controles", 90, glui_react );
      glui_controlWindow->add_column_to_panel( p_boutons );
      glui_controlWindow->add_button_to_panel( p_boutons, "Quitter", 0, selectMenu );
  }

#endif
#endif
