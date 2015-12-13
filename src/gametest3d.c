/**
 * gametest3d
 * @license The MIT License (MIT)
 *   @copyright Copyright (c) 2015 EngineerOfLies
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 *    The above copyright notice and this permission notice shall be included in all
 *    copies or substantial portions of the Software.
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *    SOFTWARE.
 */
#include "simple_logger.h"
#include "vector.h"
#include "graphics3d.h"
#include "shader.h"
#include "obj.h"
#include "sprite.h"
#include "math.h"
#include "entity.h"
#include "collisions.h"
#include "space.h"
#include "mgl_callback.h"

void set_camera(Vec3D position, Vec3D rotation);
 Vec3D cameraPosition = {0,0,0};
 Vec3D cameraRotation = {90,0,0};

 float rotation_impulse;

 const Uint8 *key_state;

 void resetCamera(Vec3D targetPosition, Vec3D targetRotation)
 {
	//Vec3D forward = Normalize();
	cameraRotation = targetRotation;
	
 }

 Entity* car;
 Entity* track;
 
 int main(int argc, char *argv[])
{
    GLuint vao;
    float r = 0;
    GLuint triangleBufferObject;
    char bGameLoopRunning = 1;

	int brake = 0;

	int current_time;

	int last_time;

    SDL_Event e;
    Obj *obj,*bgobj;
    Sprite *texture,*bgtext;
    const float triangleVertices[] = {
        0.0f, 0.5f, 0.0f, 1.0f,
        0.5f, -0.366f, 0.0f, 1.0f,
        -0.5f, -0.366f, 0.0f, 1.0f,
        //next part contains vertex colors
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f  
    }; //we love you vertices!

	entity_init(255);

	car = entity_new();

	track = entity_new();

	key_state = SDL_GetKeyboardState(NULL);
    
    init_logger("gametest3d.log");
    if (graphics3d_init(1280,960,1,"gametest3d",16) != 0)
    {
        return -1;
    }
    model_init();
    obj_init();
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); //make our vertex array object, we need it to restore state we set after binding it. Re-binding reloads the state associated with it.
    
    glGenBuffers(1, &triangleBufferObject); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //formatting the data for the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers

	entity_obj_load(car,"models/cube.obj");
	entity_load_sprite(car,"models/cube_text.png",1024,1024);

	entity_obj_load(track,"models/track.obj");
	entity_load_sprite(track,"models/track_text.png",1024,1024);

	track->rotation.x = 90;
	track->rotation.y = 90;
	
	bgobj = obj_load("models/grass.obj");
    bgtext = LoadSprite("models/grass_text.png",1024,1024);

    rotation_impulse = 0;

	current_time = SDL_GetTicks();

    while (bGameLoopRunning)
    {
		last_time = current_time;
		current_time = SDL_GetTicks();

		car->acceleration.y = 0;

		rotation_impulse = 0;

		brake = 0;

		SDL_PumpEvents();

		SDL_PollEvent(&e);

		 if (e.type == SDL_QUIT)
                bGameLoopRunning = 0;

         else if (key_state[SDL_SCANCODE_ESCAPE])
         {
			 bGameLoopRunning = 0;
		 }

		 if (key_state[SDL_SCANCODE_W])
		 {
			 car->acceleration.y = 1;
			 slog("accelerate forwards");
		 }
		 if (key_state[SDL_SCANCODE_S])
		 {
			 car->acceleration.y = -0.1;
			 slog("accelerate backwards");
		 }
		 if (key_state[SDL_SCANCODE_SPACE])
		 {
			 brake = 1;
		 }

		 if (key_state[SDL_SCANCODE_LEFT])
		 {
			 rotation_impulse = 1;
             slog("rotate left");
             slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
		 }
		  if (key_state[SDL_SCANCODE_RIGHT])
		 {
			 rotation_impulse = -1;
             slog("right");
             slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
		 }

		car->body.velocity.y += car->acceleration.y*6*(current_time - last_time)/1000.0f;

		if (brake == 1)
		{
			if (car->body.velocity.y > 0)
			{
				car->body.velocity.y -= 4*(current_time - last_time)/1000.0f;
				if  (car->body.velocity.y < 0) car->body.velocity.y = 0;
			}
			else if (car->body.velocity.y < 0)
			{
				car->body.velocity.y += 4*(current_time - last_time)/1000.0f;
				if  (car->body.velocity.y > 0) car->body.velocity.y = 0;
			}
			else {car->body.velocity.y = 0;}
		}

		car->body.velocity.y *= 0.99;

		slog("(%f)",car->body.velocity.y);

		car->rotation.z += rotation_impulse*150*(current_time - last_time)/1000.0f;

        cameraRotation.z = car->rotation.z;

		if (car->body.velocity.y > 0)
        {
            vec3d_add(
                cameraPosition,
                cameraPosition,
				vec_scale(
                vec3d(-sin(cameraRotation.z * DEGTORAD),cos(cameraRotation.z * DEGTORAD),0),
				car->body.velocity.y)
				);
			vec3d_add(
                car->body.position,
                car->body.position,
                vec_scale(
				vec3d(-sin(car->rotation.z * DEGTORAD),cos(car->rotation.z * DEGTORAD),0),
				car->body.velocity.y)
				);
            slog("(%f,%f,%f)",car->body.position.x,car->body.position.y,car->body.position.z);
        }

        else if (car->body.velocity.y < 0)
        {
            vec3d_add(
                cameraPosition,
                cameraPosition,
				vec_scale(
                vec3d(sin(cameraRotation.z * DEGTORAD),-cos(cameraRotation.z * DEGTORAD),0),
				-car->body.velocity.y)
				);
			vec3d_add(
                car->body.position,
                car->body.position,
                vec_scale(
				vec3d(sin(car->rotation.z * DEGTORAD),-cos(car->rotation.z * DEGTORAD),0),
				-car->body.velocity.y)
				);
            slog("(%f,%f,%f)",car->body.position.x,car->body.position.y,car->body.position.z);
        }

		vec3d_add(
            cameraPosition,
            cameraPosition,
            vec_scale(vec3d(sin(cameraRotation.z * DEGTORAD),-cos(cameraRotation.z * DEGTORAD),0), 10)
		);

		cameraPosition.z = 5;

		cameraRotation.x = 70;

        graphics3d_frame_begin();

        glPushMatrix();

		draw_title();

		glPushMatrix();
       
        set_camera(
            cameraPosition,
            cameraRotation);		
		
        obj_draw(
            bgobj,
            vec3d(0,0,0),
            vec3d(90,90,0),
            vec3d(1,1,1),
            vec4d(1,1,1,1),
            bgtext
        );

		
		entity_draw(track);

		entity_draw(car);

		glPopMatrix();

		draw_speed();

        glPopMatrix();

        /* drawing code above here! */

        graphics3d_next_frame();

		//cameraPosition.z = 0;

		cameraRotation.x = 90;

		cameraPosition = car->body.position;
    } 
    return 0;
}

void set_camera(Vec3D position, Vec3D rotation)
{
    glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(-rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(-rotation.z, 0.0f, 0.0f, 1.0f);
    glTranslatef(-position.x,
                 -position.y,
                 -position.z);
}

/*eol@eof*/