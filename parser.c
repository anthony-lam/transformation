#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         translate: create a translation matrix,
                    then multiply the transform matrix by the translation matrix -
                    takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
  struct matrix * transform, 
  struct matrix * edges,
  screen s) {

  FILE *f;
  char line[256];
  color c;
  c.red = 0;
  c.green = MAX_COLOR;
  c.blue = 0;
  clear_screen(s);
  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    if (!strcmp(line, "line")) {
      char * args = (char *) malloc(255);
      fgets(args, 255, f);
      double * arr = calloc(sizeof(double), 10);
      int i = 0;
      while (args) {
        arr[i] = (double) atoi(strsep(&args, " "));
        i++;
      }

      add_edge(edges, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
    }
    if (!strcmp(line, "ident")) {
      ident(transform);
    }
    if (!strcmp(line, "scale")) {
      char * args = (char *) malloc(255);
      fgets(args, 255, f);
      double * arr = calloc(sizeof(double), 10);
      int i = 0;
      while (args) {
        arr[i] = (double) atoi(strsep(&args, " "));
        i++;
      }
      struct matrix * m;
      m = make_scale(arr[0], arr[1], arr[2]);
      
      matrix_mult(m, transform);
    }
    if (!strcmp(line, "move")) {
      char * args = (char *) malloc(255);
      fgets(args, 255, f);
      double * arr = calloc(sizeof(double), 10);
      int i = 0;
      while (args) {
        arr[i] = (double) atoi(strsep(&args, " "));
        i++;
      }

      struct matrix * m;
      m = make_translate(arr[0], arr[1], arr[2]);

      matrix_mult(m, transform);
    }
    if (!strcmp(line, "rotate")) {
      char * args = (char *) malloc(255);
      fgets(args, 255, f);
      char * axis = (char *) malloc(255);
      strcpy(axis, strsep(&args, " "));
      double theta = (double) atoi(strsep(&args, " "));      
      struct matrix * m;
      if (!strcmp(axis, "x")) {
        m = make_rotX(theta);
      }
      else if ( strcmp(axis, "y") == 0 ) {
        m = make_rotY(theta);
      }
      else {
        m = make_rotZ(theta);
      }
      matrix_mult(m, transform);
    }
    if (!strcmp(line, "apply")) {
      matrix_mult(transform, edges);
    }
    if (!strcmp(line, "display")) {
      clear_screen(s);
      draw_lines(edges, s, c);
      display(s);
    }
    if (!strcmp(line, "save")) {
      char * filename = (char *) malloc(255);
      fgets(filename, 255, f);
      filename[strlen(filename)-1]='\0';
      clear_screen(s);
      draw_lines(edges, s, c);
      save_extension(s, filename);
    }
    if ( strcmp(line, "quit") == 0 ) {
      fseek(f, 0, SEEK_END);
    }
  }
}