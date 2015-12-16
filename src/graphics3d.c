#include "vector.h"
#include "graphics3d.h"
#include "simple_logger.h"
#include "shader.h"
#include <GL/glu.h>
#include <SDL_image.h>
#include "entity.h"


static SDL_GLContext __graphics3d_gl_context;
static SDL_Window  * __graphics3d_window = NULL;
static GLuint        __graphics3d_shader_program;
static Uint32        __graphics3d_frame_delay = 16;
static GLuint		 __title_id;
static GLuint		 __speed_id;
static GLuint		 __needle_id;
static GLuint		 __1_id;
static GLuint		 __2_id;
static GLuint		 __R_id;


GLfloat needleRotation = 0;

float aspect;

int gear;

void graphics3d_close();

GLuint graphics3d_get_shader_program()
{
    return __graphics3d_shader_program;
}

void graphics3d_setup_default_light();

int graphics3d_init(float sw,float sh,int fullscreen,const char *project,Uint32 frameDelay)
{

	int Mode = GL_RGB;

    const unsigned char *version;
    GLenum glew_status;
        
	aspect = sw/sh;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        slog("failed to initialize SDL!");
        return -1;
    }
    atexit(SDL_Quit);
    __graphics3d_frame_delay = frameDelay;
    
    if(fullscreen == 1)
		{
			__graphics3d_window = SDL_CreateWindow(project?project:"gametest3d",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              sw, sh,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
	}
	else
	{
		__graphics3d_window = SDL_CreateWindow(project?project:"gametest3d",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              sw, sh,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	}
    
    
    __graphics3d_gl_context = SDL_GL_CreateContext(__graphics3d_window);

	if(IMG_Init(IMG_INIT_PNG)<0)
	{
		return 0;
	}

    if (__graphics3d_gl_context == NULL)
    {
        slog("There was an error creating the OpenGL context!\n");
        return -1;
    }
    
    version = glGetString(GL_VERSION);
    if (version == NULL) 
    {
        slog("There was an error creating the OpenGL context!\n");
        return -1;
    }
    
    SDL_GL_MakeCurrent(__graphics3d_window, __graphics3d_gl_context);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    //MUST make a context AND make it current BEFORE glewInit()!
    glewExperimental = GL_TRUE;
    glew_status = glewInit();
    if (glew_status != 0) 
    {
        slog("Error: %s", glewGetErrorString(glew_status));
        return -1;
    }
    
    
    __graphics3d_shader_program = BuildShaderProgram("shaders/vs1.glsl", "shaders/fs1.glsl");
    if (__graphics3d_shader_program == -1)
    {
        return -1;
    }
    
    
    glViewport(0,0,sw, sh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /*view angle, aspect ratio, near clip distance, far clip distance*/
    /*TODO: put near/far clip in graphics view config*/
    gluPerspective( 60, (float)sw / (float)sh, .01, 4000.0f);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glClearColor(0.0,0.0,0.0,0.0);
    glClear( 1 );
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    /*Enables alpha testing*/
/*    glAlphaFunc(GL_GREATER,0);
    glEnable(GL_ALPHA_TEST);*/
    
    graphics3d_setup_default_light();
    atexit(graphics3d_close);

	bind_image("title.png", &__title_id);

	bind_image("speed_white.png", &__speed_id);

	bind_image("needle_white.png", &__needle_id);

	bind_image("1.png", &__1_id);

	bind_image("2.png", &__2_id);

	bind_image("R.png", &__R_id);

    return 0;
}

void graphics3d_close()
{
    SDL_GL_DeleteContext(__graphics3d_gl_context);
}

void graphics3d_frame_begin()
{
    glClearColor(0.0,0.0,0.0,0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    glPushMatrix();
}

void graphics3d_next_frame()
{
    static Uint32 then = 0;
    Uint32 now;
    glPopMatrix();
    SDL_GL_SwapWindow(__graphics3d_window);
    now = SDL_GetTicks();
    if ((now - then) < __graphics3d_frame_delay)
    {
        SDL_Delay(__graphics3d_frame_delay - (now - then));        
    }
    then = now;
}

void graphics3d_setup_default_light()
{
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { -10.0, -10.0, 10.0, 0.0 };
    GLfloat light_ambient[] = { 3.0, 3.0, 3.0, 3.0 };//made everything brighter
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    
    GLfloat light1_ambient[] = { 1.2, 1.2, 1.2, 1.0 };

    GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light1_position[] = { -20.0, 2.0, 1.0, 1.0 };
    GLfloat spot_direction[] = { -1.0, -1.0, 0.0 };
    
    GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    
    //glShadeModel (GL_SMOOTH);
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.9);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.7);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.3);
    
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 80.0);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
    
}


/*eol@eof*/


void draw_title()
{
	Vec3D verts[4];

	Vec2D UVs[4];

	verts[0].x = 1;
	verts[0].y = 1;
	//verts[0].z = -1;

	UVs[0].x = 1;
	UVs[0].y = 1;

	verts[1].x = -1;
	verts[1].y = 1;
	//verts[1].z = -1;

	UVs[1].x = 0;
	UVs[1].y = 1;

	verts[2].x = -1;
	verts[2].y = -1;
	//verts[2].z = -1;

	UVs[2].x = 0;
	UVs[2].y = 0;

	verts[3].x = 1;
	verts[3].y = -1;
	//verts[3].z = -1;

	UVs[3].x = 1;
	UVs[3].y = 0;



	//glPushMatrix();

	orthogonalStart();

	glDisable(GL_DEPTH_TEST);     
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,__title_id);
    
    glBegin(GL_TRIANGLES);



	     
    glTexCoord2f(UVs[0].x,UVs[0].y);
    glVertex2f(verts[0].x,verts[0].y);

	glTexCoord2f(UVs[1].x,UVs[1].y);
    glVertex2f(verts[1].x,verts[1].y);

	glTexCoord2f(UVs[2].x,UVs[2].y);
    glVertex2f(verts[2].x,verts[2].y);

	glTexCoord2f(UVs[0].x,UVs[0].y);
    glVertex2f(verts[0].x,verts[0].y);

	glTexCoord2f(UVs[2].x,UVs[2].y);
    glVertex2f(verts[2].x,verts[2].y);

	glTexCoord2f(UVs[3].x,UVs[3].y);
    glVertex2f(verts[3].x,verts[3].y);
        
    glEnd();
    
   
    glDisable(GL_TEXTURE_2D);

    //glPopMatrix();

	orthogonalEnd();
}

void orthogonalStart()
{
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

	glLoadIdentity();

	gluOrtho2D(-1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}

void orthogonalEnd()
{
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

void draw_speed()
{
	needleRotation = car->body.velocity.y/10.0*270.0;
	if(needleRotation < 0)needleRotation *= -1;
	drawQuad(vec2d(.5f,.5f), vec2d(.75f,-.75f), __speed_id, 0, 1);
	drawQuad(vec2d(.5f,.5f), vec2d(.75f,-.75f), __needle_id, -needleRotation, 1);
}

void draw_gear()
{
	if(gear == 1)drawQuad(vec2d(.25f,.35f), vec2d(-.75f,-.75f), __1_id, 0, 1);
	else if(gear == 2)drawQuad(vec2d(.25f,.35f), vec2d(-.75f,-.75f), __2_id, 0, 1);
	else if(gear == 0)drawQuad(vec2d(.25f,.35f), vec2d(-.75f,-.75f), __R_id, 0, 1);
}

void drawQuad(Vec2D size, Vec2D pos, GLuint tex_id, GLfloat angle, int depthTest)
{
	Vec2D verts[4];

	Vec2D UVs[4];


	//upper right
	verts[0].x = size.x/2/aspect;
	verts[0].y = size.y/2;

	UVs[0].x = 1;
	UVs[0].y = 0;

	//upper left
	verts[1].x = -size.x/2/aspect;
	verts[1].y = size.y/2;

	UVs[1].x = 0;
	UVs[1].y = 0;

	//bottom left
	verts[2].x = -size.x/2/aspect;
	verts[2].y = -size.y/2;

	UVs[2].x = 0;
	UVs[2].y = 1;

	//bottom right
	verts[3].x = size.x/2/aspect;
	verts[3].y = -size.y/2;

	UVs[3].x = 1;
	UVs[3].y = 1;

	orthogonalStart();

	glTranslatef(pos.x, pos.y, 0);
	glRotatef(angle, 0, 0, 1);
	

	if(depthTest)glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex_id);

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    glBegin(GL_TRIANGLES);

	     
    glTexCoord2f(UVs[0].x,UVs[0].y);
    glVertex2f(verts[0].x,verts[0].y);

	glTexCoord2f(UVs[1].x,UVs[1].y);
    glVertex2f(verts[1].x,verts[1].y);

	glTexCoord2f(UVs[2].x,UVs[2].y);
    glVertex2f(verts[2].x,verts[2].y);

	glTexCoord2f(UVs[0].x,UVs[0].y);
    glVertex2f(verts[0].x,verts[0].y);

	glTexCoord2f(UVs[2].x,UVs[2].y);
    glVertex2f(verts[2].x,verts[2].y);

	glTexCoord2f(UVs[3].x,UVs[3].y);
    glVertex2f(verts[3].x,verts[3].y);
        
    glEnd();
    
   
    glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	orthogonalEnd();
}

void bind_image(char* file, GLuint *id)
{
	int Mode = GL_RGB;

	SDL_Surface * img;

	img = IMG_Load(file);

	glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D,*id);
    
    
    if(img->format->BytesPerPixel == 4) {
        Mode = GL_RGBA;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, Mode, img->w, img->h, 0, Mode, GL_UNSIGNED_BYTE, img->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
}