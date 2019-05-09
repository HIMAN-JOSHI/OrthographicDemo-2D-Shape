// This program is a demo for orthographic projection using a 2D object (triangle).

#include <stdio.h>  
#include <stdlib.h> // for exit()
#include <memory.h> // for memset()

#include <X11/Xlib.h> // for native windowing
#include <X11/Xutil.h> // for the visual and related api
#include <X11/XKBlib.h> // for keyboard api - XkbKeycodeToKeysym()

#include <GL/gl.h>
#include <GL/glx.h> // for bridging-api
#include <GL/glu.h> // for OpenGL

//global variable declarations
bool gbFullscreen=false;
int giWindowWidth=800;
int giWindowHeight=600;
Display *gpDisplay = NULL; // 'Display' is a struct (or connection) used by the X client to communicate with the X server (O.S)
XVisualInfo *gpXVisualInfo=NULL; // 'XVisualInfo' is a struct that stores information about the visual.
Colormap gColormap; // color palette
Window gWindow;
GLXContext gGLXContext; // GLXContext is a data structure which acts like a state machine that stores all the states associated with this instance of  OpenGL.

// entry point function
int main(void){
	
	// funciton prototypes
	void createWindow(void);
	void toggleFullscreen(void);
	void initialize(void);
	void display(void);
	void resize(int, int);
	void uninitialize(void);

	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;
	
	bool bDone=false; // this flag will be used to 'close' the window
	
	// code
	
	// 1. Create the window
	createWindow();

	// 2. Initialize OpenGL
	initialize();

	// Message Loop
	XEvent event;
	KeySym keysym;

	while(bDone==false){

		while(XPending(gpDisplay)){ // XPending() func. returns the number of events that have been received from the X server but have not been removed from the event queue. gpDisplay - connection to the X server
			
			XNextEvent(gpDisplay, &event); // XNextEvent() - Copies the first event from the event queue into the specified 'event' obj and then removes it from the event queue.

			switch(event.type){
				case MapNotify: // Similar to WM_CREATE. This event is generated when the window changes state from unmapped to mapped
					break;
				case KeyPress: // handles keyboard button press
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0); // 3rd param - locale, 0 (default), 4th param - Shift key status 0 (shift not used)
					switch(keysym){
						case XK_Escape: // XK - X Keycode
							uninitialize();
							exit(0); 
							// OR
							// bDone = true;
						case XK_F:
						case XK_f:
							if(gbFullscreen==false){
								toggleFullscreen();
								gbFullscreen = true;
							}
							else{
								toggleFullscreen();
								gbFullscreen = false;
							}		
						break;
						default:
							break;
					}// switch(keysym)
					break;

				case ButtonPress: // Mouse button events
					switch(event.xbutton.button){
						case 1: // similar to WM_LBUTTONDOWN
							break;
						case 2: // similar to WM_MBUTTONDOWN
							break;
						case 3: // similar to WM_RBUTTONDOWN
							break;
						case 4: // similar to MOUSE_WHEELUP
							break;
						case 5: // similar to MOUSE_WHEELDOWN
							break;
					} // END: switch(event.xbutton.button)
					break;
				case MotionNotify: // WM_MOUSEMOVE
					break;
				case ConfigureNotify: // similar to WM_RESIZE
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
				case Expose: // similar to WM_PAINT
					break;
				case DestroyNotify: // similar to WM_DESTROY
					break;
				case 33: // handles click on 'Close' box and also a click on sys menu 'Close' (this number never changes)
					// Note:- Preparation for this case has been done in createWindow(...) by creating an atom and setting the WM protocols.
				uninitialize();
				exit(0);
				default:
					break;
			}// switch(event.type)				

		} // while (XPending(gpDisplay))

		display(); // Rendering is done here

	}// while (bDone==false)
	
	uninitialize();
	return(0); 
}

void display(void){

	// code
	
	// Step 1: Clear all the pixels
	glClear(GL_COLOR_BUFFER_BIT);

	//###### TRIANGLE #####
	// Step 2: Begin paint
	glBegin(GL_TRIANGLES);
		// white: Colour of the triangle
		glColor3f(1.0f, 1.0f, 1.0f);
		//co-ordinates of the triangle
		glVertex3f(0.0f,50.0f,0.0f); // apex of the triangle
		glVertex3f(-50.0f, -50.0f, 0.0f); // left-bottom tip of the triangle
		glVertex3f(50.0f,-50.0f,0.0f); // right-bottom tip of the triangle
	glEnd(); // End paint

	// Step 3: Process buffered OpenGL Routines
	glFlush(); // Empty the buffer
}

void initialize(void){

	// function prototype
	void resize(int, int);

	// code
	// Step 1: Create an OpenGL context
	/* Func: glXCreateContext() creates a GLX rendering context and returns its handle.
	   Parameters:  Display - Specifies the connection to the X server.
			XVisualInfo - Specifies the visual that defines the frame buffer resources available to the rendering context.
			GLXContext - Specifies the context with which to share display list. NULL - The context is not shared
			Bool - Specifies the direct rendering. True - Hardware rendering using the graphics card. False - Software rendering using the X-server (O.S)
	*/
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);

	// Step 2: Make the 'gGLXContext' created above as the current context
	glXMakeCurrent(gpDisplay,gWindow, gGLXContext);

	// Set clear values [0-1] to the color buffers
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	
	resize(giWindowWidth, giWindowHeight);
}

void resize(int width, int height){

	//code
	if(height==0)
		height=1;

	if(width==0)
		width=1;	
	
	// set the viewport according to newer size of the window
	// Note:- Viewport is the actual window where the pciture is drawn.
	glViewport(0,0, (GLsizei) width, (GLsizei) height); // 1st param - x, 2nd param - y, 3rd param - width, 4th param - height

	// Specify which matrix is the current matrix
	glMatrixMode(GL_PROJECTION); // GL_PROJECTION - Deals with the volume of the view.

	// Convert the above projection matrix to identity matrix
	glLoadIdentity();

	// glOrtho(left, right, bottom, top, near, far)
	if(width<=height){
		glOrtho(-100.0f, 100.0f, (-100.0f * ((GLfloat)height/(GLfloat)width)), (100.0f * ((GLfloat)height/(GLfloat)width)), -100.0f, 100.0f); // co-ordinates written for glVertex3f() are relative to viewing volume
	}	
	else{
		glOrtho((-100.0f * ((GLfloat)width/(GLfloat)height)), (100.0f * ((GLfloat)width/(GLfloat)height)), -100.0f, 100.0f, -100.0f, 100.0f); // co-ordinates written for glVertex3f() are relative to viewing volume
	}

	glMatrixMode(GL_MODELVIEW); // Model view matrix is in the middle of the screen and all the drawing is done here
	glLoadIdentity();
}

void toggleFullscreen(void){

	// variable declaration
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {0};

	//code
	// Step 1: Get the current placement of the window and save it in an atom	
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False); // '_NET_WM_STATE' - Network compliant, False - don't create if this atom exists
	
	// Step 2: Create a custom/client-event
	// Step 2.1: Allocate 0 memory to all the members of the 'event' obj first
	memset(&xev,0,sizeof(xev));

	// Step 2.2: Set the values to this custom/client-event
	xev.type = ClientMessage;
	xev.xclient.window = gWindow; 
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32; // 32-bit
	// Note:- "data" is a union in XEvent and "l" is the sub-union in it.
	xev.xclient.data.l[0] = gbFullscreen ? 0 : 1; // Check the value present in 'l[0]' and perform the corresponding action in 'l[1]'.
	
	// Step 3: Create an atom for fullscreen
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False); // '_NET_WM_STATE_FULLSCREEN' -> atom related to fullscreen, False -> Don't create a new atom if it already exists.
	
	xev.xclient.data.l[1] = fullscreen; // all 'fullscreen' atom to l[1].

	// Step4: Send the custom/client-event to the queue.
	XSendEvent(gpDisplay,
                   RootWindow(gpDisplay, gpXVisualInfo->screen), // Propogate the message to this window
		   False, // Do not propogate the message to the child window
		   StructureNotifyMask, // similar to WM_SIZE
		   &xev); // custom-event created above		
}

void createWindow(void){ // Note:- all the programs which create their own window are XClients of the XServer (i.e. O.S)

	// function prototypes
	void uninitialize(void);

	// variable declaration
	XSetWindowAttributes winAttribs;

	int defaultScreen;
	int styleMask;

	static int frameBufferList[] = {
		GLX_RGBA, // consider only TrueColor and DirectColor visuals (otherwise PseudoColor and StaticColor visuals are considered).
		GLX_RED_SIZE, 1, // The returned visual should support a buffer (1-bit atleast) of red color.
		GLX_GREEN_SIZE, 1, // The returned visual should support a buffer (1-bit atleast) of green color.
		GLX_BLUE_SIZE, 1, // The returned visual should support a buffer (1-bit atleast) of blue color.
		GLX_ALPHA_SIZE, 1, // The returned visual should support a buffer (1-bit atleast) of alpha.
		None // The framebufferAttribute array must be terminated by 0 and hence 'None' is used.
	};		

	// code
	// Step 1: Get the connection of the local display.
	gpDisplay = XOpenDisplay(NULL); // NULL - Gives default local connection.
	if(gpDisplay==NULL){
		printf("ERROR: Unable to open X-Display. \nExiting now...");
		uninitialize();
		exit(1); // Abortive exit and hence a positive number.
	}
	
	// Step 2: Get the default monitor/screen (from 'Display' struct) to which the graphic card is connected.
	defaultScreen = XDefaultScreen(gpDisplay);

	// Step 3: Get the XVisualInfo struct obj that meets the minimum requirements.
	// Note:- A single display can support multiple screens. Each screen can have several different visual types supported at different depths
	gpXVisualInfo=glXChooseVisual(gpDisplay, defaultScreen, frameBufferList); // glXChooseVisual() returns a pointer to an XVisualInfo structure that best meets the specified requirements.

	// Step 4: Set the window attributes
	winAttribs.border_pixel = 0; // 0 - default border color
	winAttribs.background_pixmap = 0; // background pixmap - images like cursor, icon, etc. (0 - default)
	
	// Get the colormap ID.
	// Note:- The XCreateColormap() func creates a colormap of the specified visual type for the screen on which the spcified window resides and returns the colormap ID associated with it.
	winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone); // AllocNone - Dont allocate fixed memory.

	gColormap = winAttribs.colormap;
	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen); // set the background color
		
	// Specify the events which should be sent to this window
	winAttribs.event_mask = ExposureMask | // similar to WM_PAINT
				VisibilityChangeMask | // similar to WM_CREATE
				ButtonPressMask | // handles mouse button events
				KeyPressMask | // handles key board key press events
				PointerMotionMask | // handles mouse motion events
				StructureNotifyMask; // case ConfigureNotify: handles window resize, similar to WM_SIZE
	
	// Specify the window style	
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap; // CW - Create Window or Change Window

	// Step 5: Create the window
	gWindow = XCreateWindow(gpDisplay, // specifies the connection with the X-server
				RootWindow(gpDisplay, gpXVisualInfo->screen), // specifies the parent window
				0, // x-coordinate
				0, // y-coordinate
				giWindowWidth, // width
				giWindowHeight, // height
				0, // border-width (0 - default)
				gpXVisualInfo->depth, // depth of the window. Color depth
				InputOutput, // type of the window
				gpXVisualInfo->visual, // type of visual
				styleMask, // style
				&winAttribs); // attributes of the window

	if(!gWindow){
		printf("ERROR: Failed to create the window. \nExiting now...\n");
		uninitialize();
		exit(1); // Abortive exit and hence a positive integer is used.
	}

	// Step 6: Name in the caption bar
	XStoreName(gpDisplay, gWindow, "Demo - One 2D shape orthographic projection");

	// Step 7: Process the window close event

	// Step 7.1: Create an atom to handle the close window event
	// Note:- 'Atom' is a unique string and it stays in the memory until the app ends
	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True); // XInternAtom returns the atom id associated with "WM_DELETE_WINDOW" message (WM stands for Window Message), True - create an atom even if it exists.

	// Step 7.2: Ask the window manager to add the 'windowManagerDelete' atom to the protocols.
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete, 1); // 3rd param is an array of atoms, 4th param - no. of protocols to set

	// Step 8: Map this window to the screen
	XMapWindow(gpDisplay, gWindow);
}

void uninitialize(void){ // works like a destructor i.e. destroy in the reverse order of creation

	GLXContext currentGLXContext;
	currentGLXContext = glXGetCurrentContext();

	if((currentGLXContext!=NULL) && (currentGLXContext==gGLXContext)){
		glXMakeCurrent(gpDisplay,0,0);
	}
	if(gGLXContext){
		glXDestroyContext(gpDisplay, gGLXContext);
	}
	if(gWindow){
		XDestroyWindow(gpDisplay, gWindow);
	}
	if(gColormap){
		XFreeColormap(gpDisplay, gColormap);
	}
	if(gpXVisualInfo){

		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	if(gpDisplay){
	
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
}
