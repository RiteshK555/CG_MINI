#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <math.h>
#include "bits/stdc++.h"
#include "iostream"
using namespace std;
#define BOUNDS  1



#define WATERSIZE 200
#define DAMP 20

float water[2][WATERSIZE][WATERSIZE];


int spin_x, spin_y, spin_z; /* x-y rotation and zoom */
int h, w;                    /* height, width  of window */
int old_x, old_y, move_z;
int depth = 3;
int i = 0;

int t = 0, f = 1;

void calcwater() {
	int x, y;
	float n;
    int b = 2;
	for(y = 1; y < WATERSIZE-1; y++) {
		for(x = 1; x < WATERSIZE-1; x++) {
			n = ( water[t][x-1][y] +
				  water[t][x+1][y] + 
				  water[t][x][y-1] + 
				  water[t][x][y+1]
				  ) / b;
			n -= water[f][x][y];
      		n = n - (n / DAMP);
			water[f][x][y] = n;
		}
	}

	y = 0;
	for(x = 1; x < WATERSIZE-1; x++) {
			n = ( water[t][x-1][y] +
				  water[t][x+1][y] + 
				  water[t][x][y+1]
				  ) / b;
			n -= water[f][x][y];
      		n = n - (n / DAMP);
			water[f][x][y] = n;
	}
	
	
	x = 0;
	for(y = 1; y < WATERSIZE-1; y++) {
			n = ( water[t][x+1][y] + 
				  water[t][x][y-1] + 
				  water[t][x][y+1]
				  ) / b;
			n -= water[f][x][y];
      		n = n - (n / DAMP);
			water[f][x][y] = n;
	}

	x = WATERSIZE-1;
	for(y = 1; y < WATERSIZE-1; y++) {
			n = ( water[t][x-1][y] +
				  water[t][x][y-1] + 
				  water[t][x][y+1]
				  ) / b;
			n -= water[f][x][y];
      		n = n - (n / DAMP);
			water[f][x][y] = n;
	}
	y = WATERSIZE-1;
	for(x = 1; x < WATERSIZE-1; x++) {
			n = ( water[t][x-1][y] +
				  water[t][x+1][y] + 
				  water[t][x][y-1] 
				  ) / b;
			n -= water[f][x][y];
      		n = n - (n / DAMP);
			water[f][x][y] = n;
	}

}


void reshape(int width, int height) {
    w = width;
    h = height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat) w/(GLfloat) h, 1.0, 2000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();
}

void display(void) {
    int i, j, tmp;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glPushMatrix();

    glTranslatef(0, 0, spin_z-220);

    glRotatef(spin_x, 0, 1, 0);


    glRotatef(spin_y-60, 1, 0, 0);
    // cout<<"spinx and spiny"<<spin_x <<" "<<spin_y<<endl;
    
	calcwater();
	glBegin(GL_POINTS);
	for(i = 0; i < WATERSIZE; i++) {
		for(j = 0; j < WATERSIZE; j++) {
				glColor3f(0,0,1);
			glVertex3f(j-WATERSIZE/2, i-WATERSIZE/2, water[t][j][i]);
		}
	}
	glEnd();

	tmp = t; t = f; f = tmp;


    glPopMatrix();

    
    glutSwapBuffers();
}

// int num  = 0;
// int delay = 70;
void idle(void)
{
	
	// if(!(++num %delay))
	// {
	// 	water[f][rand()%WATERSIZE][rand()%WATERSIZE] = -rand()%200;
	// 	delay = rand()%100 + 50;
	// }
    glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON){
        old_x = x - spin_x;
        old_y = y - spin_y;
        if(state == GLUT_DOWN){
            // cout<<x<<" "<<y<<endl;
            GLint viewport[4];
            GLdouble modelview[16];
            GLdouble projection[16];
            GLdouble winx, winy, winz;
            GLdouble posx, posy, posz;

            glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
            glGetDoublev(GL_PROJECTION_MATRIX, projection);
            glGetIntegerv(GL_VIEWPORT, viewport);

            winx = (double)x;
            winy = (double)viewport[3] - (double)y;
            // winx = 512.0 - (double)x;
            // winy = (double)y;
            glReadPixels(x,int(winy),1,1,GL_DEPTH_COMPONENT,GL_DOUBLE,&winz);

            gluUnProject(
                winx,winy,0,
                modelview,projection,viewport,
                &posx,&posy,&posz
            );
            
            double x1 = posx, y1 = posy, z1 = posz;

            gluUnProject(
                winx,winy,1,
                modelview,projection,viewport,
                &posx,&posy,&posz
            );

            double x2 = posx, y2 = posy, z2 = posz;
    
            
            //angles

            float alpha = ((spin_y - 60) * M_PI) / 180.0;
            float beta = (spin_x * M_PI) / 180.0;

            double sa = sin(alpha);
            double sb = sin(beta);
            double ca = cos(alpha);
            double cb = cos(beta);

            cout<<"--------------angles--------------"<<endl;
            cout<<ca<<" "<<cb<<" "<<sa<<" "<<sb<<endl;
            //eqn of plane 
            // (ca* sb)*(x) + (-sa)*(y) + (cb* ca) * (z + 220) = 0
            cout<<"-------------eqn of plane----------"<<endl;
            cout<<(ca*sb)<<" "<<(-sa)<<" "<<(cb*ca)<<endl;

            //ignore

            //x = x1 + r * d1
            //y = y1 + r * d2
            //z = z1 + r * d3
            // r = -ca * sb * x1  + sa * y1 - cb * ca * ( z1 + 220)
                   //----------------------------------------------
            //      ca * sb * d1 - sa * d2 + cb * ca * d3

            // float d = sqrt((x2-x1)* (x2-x1) + (y2 - y1) * (y2 - y1) + (z2 - z1)* (z2-z1));
            // float d1 = (x2 - x1)/ d;
            // float d2 = (y2 - y1)/ d;
            // float d3 = (z2 - z1)/ d;
            // float r = (-ca * sb * x1 + sa * y1 - cb * ca * (z1 + 220))/ (ca * sb * d1 - sa * d2 + cb * ca * d3);
            // float a = x1 + r * d1, b = y1 + r * d2, c = z1 + r * d3;


            //correct 

            // x = r * x1 + (1-r) * x2;
            // y = r * y1 + (1-r) * y2;
            // z = r * z1 + (1-r) * z2;
            float r = ((-ca*sb)*x2 + sa * y2 - (cb * ca)*(z2 + 220))/((ca*sb)*(x1-x2) + (-sa)*(y1-y2) + (cb*ca)*(z1-z2));
            float a = x1 * r + (1-r)* x2, b = y1 * r + (1-r) * (y2), c = z1 * r + (1-r)*z2; 

            //ignore
            //translate back to original
            // z += 220
            // fx = (cb * x + sa * sb * y - ca * sb * z)
            // fy = (ca * y + sa * z)
            // fz = (sb * x - sa * cb * y + ca * cb * z)

            //final
            // float beta = ((spin_y - 60) * M_PI)/ 360.0;
            // float alpha = (spin_x * M)
            

            //ignore

            // float fx = (cb * a + sa * sb * b - ca * sb * c);
            // float fy = (ca * b + sa * c);
            // float fz = (sb * a - sa * cb * b + ca * cb * c);


            // correct

            c += 220;
            float fx = (cb * a + 0 * b + (-sb) * (c));
            float fy = (sa * sb * a + ca * b + sa * cb * c);
            float fz = (ca * sb * a + (-sa) * (b) + ca * cb * c);


            fx += WATERSIZE/2; fy += WATERSIZE/2;
            // cout<<x<<" "<<y<<endl; 
            cout<<"-------- NP AND FP -----------"<<endl;
            cout<<x1<<" "<<y1<<" "<<z1<<endl;
            cout<<x2<<" "<<y2<<" "<<z2<<endl;

            cout<<"--------plane points-----------"<<endl;
            cout<<fx<<" "<<fy<<" "<<fz<<endl;

            cout<<"--------ratio------------------"<<endl;
            cout<<r<<endl;

            if(fx>=0 and fy>=0 and fx<=200 and fy<=200)
                water[f][(int)fx][(int)fy] = -200; 
            glutPostRedisplay();
        }
    }      
    else if(button == GLUT_RIGHT_BUTTON){
        old_y = y - spin_z;
        move_z = (move_z ? 0 : 1);
    }
    glutPostRedisplay();

}

void 
motion(int x, int y) {

    if(!move_z) {
        spin_x = x - old_x;
        spin_y = y - old_y;
    } else {
        spin_z = y - old_y;
    }

    glutPostRedisplay();
}


void
keyboard(unsigned char key, int x, int y)
{
    static int old_x = 50;
    static int old_y = 50;
    static int old_width = 512;
    static int old_height = 512;

    switch (key) {
        case 'x':
                exit(0);
            break;
        case 'f':
            if (glutGet(GLUT_WINDOW_WIDTH) < glutGet(GLUT_SCREEN_WIDTH)) {
                old_x = glutGet(GLUT_WINDOW_X);
                old_y = glutGet(GLUT_WINDOW_Y);
                old_width = glutGet(GLUT_WINDOW_WIDTH);
                old_height = glutGet(GLUT_WINDOW_HEIGHT);
                glutFullScreen();
            }else{
                glutReshapeWindow(old_width, old_height);
            }
            break;
		default:
            break;
    }
}


void init(void) {
	int i, j;

    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);

   // glEnable(GL_LIGHTING);
  //  glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

	for( i = 0; i < WATERSIZE; i++) 
		for( j = 0; j < WATERSIZE; j++) {
			water[0][j][i] = 0;
			water[1][j][i] = 0;
		}
}

int
main(int argc, char** argv)
{
    

   // srand(time(NULL));

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(512, 512);
    glutInit(&argc, argv);

    glutCreateWindow("Simulating Water");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glEnable (GL_DEPTH_TEST);

    if(argc == 2) {
        if (strcmp(argv[1], "-h") == 0) {
            fprintf(stderr, "%s [depth]\n", argv[0]);
            exit(0);
        }
        sscanf(argv[1], "%d", &depth);
    }

	printf("Water Simulation \n");
    init();

    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}

