/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux/SDL by Ti Leggett '01)
 *
 * If you've found this code useful, please let me know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 * or for port-specific comments, questions, bugreports etc.
 * email to leggett@eecs.tulane.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "SDL.h"

#include "FreeType.h"

/* screen width, height, and bit depth */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16

/* Define our booleans */
#define TRUE  1
#define FALSE 0

freetype::font_data our_font;

/* This is our SDL surface */
SDL_Surface * surface;

GLfloat cnt1;
GLfloat cnt2;

/* function to release/destroy our resources and restoring the old desktop */
void Quit(int returnCode)
{
	/* clean up the window */
	SDL_Quit();

	/* and exit appropriately */
	exit(returnCode);
}

/* function to reset our viewport after a window resize */
int resizeWindow(int width, int height)
{
	/* Height / width ration */
	GLfloat ratio;

	/* Protect against a divide by zero */
	if (height == 0)
		height = 1;

	ratio = (GLfloat)width / (GLfloat)height;

	/* Setup our viewport. */
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	/* change to the projection matrix and set our viewing volume. */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* Set our perspective */
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode(GL_MODELVIEW);

	/* Reset The View */
	glLoadIdentity();

	return (TRUE);
}

/* function to handle key press events */
void handleKeyPress(SDL_keysym * keysym)
{
	switch (keysym->sym) {
	case SDLK_ESCAPE:
		/* ESC key was pressed */
		Quit(0);
		break;
	case SDLK_F1:
		/* F1 key was pressed
		 * this toggles fullscreen mode
		 */
		SDL_WM_ToggleFullScreen(surface);
		break;
	default:
		break;
	}

	return;
}

/* general OpenGL initialization function */
int initGL(GLvoid)
{

	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);

	/* Set the background black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/* Depth buffer setup */
	glClearDepth(1.0f);

	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);

	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);

	/* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	our_font.init("../Test.ttf", 16);

	return (TRUE);
}

/* Here goes our drawing code */
int drawGLScene(GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
	glLoadIdentity();     // Reset The Current Modelview Matrix
	glTranslatef(0.0f, 0.0f, -1.0f);  // Move One Unit Into The Screen

	// Blue Text
	glColor3ub(0, 0, 0xff);

	// Position The WGL Text On The Screen
	// glRasterPos2f(-0.40f, 0.35f);

	// Here We Print Some Text Using Our FreeType Font
	// The only really important command is the actual print() call,
	// but for the sake of making the results a bit more interesting
	// I have put in some code to rotate and scale the text.

	// Red text
	glColor3ub(0xff, 0, 0);

	glPushMatrix();
	glLoadIdentity();
	glRotatef(cnt1, 0, 0, 1);
	glScalef(1, .8 + .3 * cos(cnt1 / 5), 1);
	glTranslatef(-180, 0, 0);
	freetype::print(our_font, 320, 240, "Active FreeType Text - %7.2f", cnt1);
	glPopMatrix();

	//Uncomment this to test out print's ability to handle newlines.
	//freetype::print(our_font, 320, 200, "Here\nthere\nbe\n\nnewlines\n.", cnt1);

	cnt1 += 0.051f; // Increase The First Counter
	cnt2 += 0.005f; // Increase The First Counter

	SDL_GL_SwapBuffers();
	return (TRUE);
}

int main(int argc, char ** argv)
{
	/* Flags to pass to SDL_SetVideoMode */
	int videoFlags;
	/* main loop variable */
	int done = FALSE;
	/* used to collect events */
	SDL_Event event;
	/* this holds some info about our display */
	const SDL_VideoInfo * videoInfo;
	/* whether or not the window is active */
	int isActive = TRUE;

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Video initialization failed: %s\n",
			SDL_GetError());
		Quit(1);
	}

	/* Fetch the video info */
	videoInfo = SDL_GetVideoInfo();

	if (!videoInfo) {
		fprintf(stderr, "Video query failed: %s\n",
			SDL_GetError());
		Quit(1);
	}

	/* the flags to pass to SDL_SetVideoMode */
	videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
	videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
	videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
	videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

	/* This checks to see if surfaces can be stored in memory */
	if (videoInfo->hw_available)
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	/* This checks if hardware blits can be done */
	if (videoInfo->blit_hw)
		videoFlags |= SDL_HWACCEL;

	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	/* get a SDL surface */
	surface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
				   videoFlags);

	/* Verify there is a surface */
	if (!surface) {
		fprintf(stderr,  "Video mode set failed: %s\n", SDL_GetError());
		Quit(1);
	}

	/* initialize OpenGL */
	initGL();

	/* resize the initial window */
	resizeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

	/* wait for events */
	while (!done) {
		/* handle the events in the queue */

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_ACTIVEEVENT:
				/* Something's happend with our focus
				 * If we lost focus or we are iconified, we
				 * shouldn't draw the screen
				 */
				if (event.active.gain == 0)
					isActive = FALSE;
				else
					isActive = TRUE;
				break;
			case SDL_VIDEORESIZE:
				/* handle resize event */
				surface = SDL_SetVideoMode(event.resize.w,
							   event.resize.h,
							   16, videoFlags);
				if (!surface) {
					fprintf(stderr, "Could not get a surface after resize: %s\n", SDL_GetError());
					Quit(1);
				}
				resizeWindow(event.resize.w, event.resize.h);
				break;
			case SDL_KEYDOWN:
				/* handle key presses */
				handleKeyPress(&event.key.keysym);
				break;
			case SDL_QUIT:
				/* handle quit requests */
				done = TRUE;
				break;
			default:
				break;
			}
		}

		/* draw the scene */
		if (isActive)
			drawGLScene();
	}

	/* clean ourselves up and exit */
	Quit(0);

	/* Should never get here */
	return (0);
}
