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
//#include "space.h"
#include "mgl_callback.h"

void set_camera(Vec3D position, Vec3D rotation);
 Vec3D cameraPosition = {0,0,0};
 Vec3D cameraRotation = {90,0,0};

float rotation_impulse;

int gear;

int lap;

const Uint8 *key_state;

 void resetCamera(Vec3D targetPosition, Vec3D targetRotation)
 {
	//Vec3D forward = Normalize();
	cameraRotation = targetRotation;
	
 }

 SDL_Joystick *joy;

 Entity* car;
 Entity* track;
 Entity* start;
 Entity* check1;
 Entity* check2;
 Entity* check3;
 
 int main(int argc, char *argv[])
{
    GLuint vao;
    GLuint triangleBufferObject;
    char bGameLoopRunning = 1;

	int brake = 0;

	int current_time;

	int last_time;

	int square;

	int cross;

	int camera_reverse;

	float camera_rotation_x;

	int checkpoint;

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

	start = entity_new();

	check1 = entity_new();

	check2 = entity_new();

	check3 = entity_new();

	key_state = SDL_GetKeyboardState(NULL);
    
    init_logger("gametest3d.log");
    if (graphics3d_init(1280,960,0,"gametest3d",16) != 0)
    {
        return -1;
    }

	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	if (SDL_NumJoysticks() > 0) {
    // Open joystick
    joy = SDL_JoystickOpen(0);

    if (joy) {
        slog("Opened Joystick 0\n");
        slog("Name: %s\n", SDL_JoystickNameForIndex(0));
        slog("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
        slog("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
        slog("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
    } else {
        slog("Couldn't open Joystick 0\n");
    }

	}

    model_init();
    obj_init();
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); //make our vertex array object, we need it to restore state we set after binding it. Re-binding reloads the state associated with it.
    
    glGenBuffers(1, &triangleBufferObject); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //formatting the data for the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers

	//entity_obj_load(car,"models/cube.obj");
	//entity_load_sprite(car,"models/cube_text.png",1024,1024);

	entity_obj_load(car,"models/blue_falcon.obj");
	entity_load_sprite(car,"models/BLF1.jpg",512,512);

	car->scale.x = 2;
	car->scale.y = 2;
	car->scale.z = 2;

	entity_obj_load(track,"models/track.obj");
	entity_load_sprite(track,"models/track_text.png",1024,1024);

	entity_obj_load(start,"models/cube.obj");
	entity_load_sprite(start,"models/finish.jpg",1024,1024);

	start->scale.x = 17;
	start->scale.z = 0.1;
	start->body.position.x = -1;

	entity_obj_load(check1,"models/cube.obj");
	entity_load_sprite(check1,"models/cube_text.png",1024,1024);

	check1->body.position.x = -300;
	check1->body.position.y = 390;

	entity_obj_load(check2,"models/cube.obj");
	entity_load_sprite(check2,"models/cube_text.png",1024,1024);

	check2->body.position.x = -600;
	check2->body.position.y = 0;

	entity_obj_load(check3,"models/cube.obj");
	entity_load_sprite(check3,"models/cube_text.png",1024,1024);

	check3->body.position.x = -300;
	check3->body.position.y = -360;

	track->rotation.x = 90;
	track->rotation.y = 90;
	
	bgobj = obj_load("models/grass.obj");
    bgtext = LoadSprite("models/grass_text.png",1024,1024);

    rotation_impulse = 0;

	gear = 1;

	square = 0;

	cross = 0;

	checkpoint = 0;

	lap = 1;

	current_time = SDL_GetTicks();

	SDL_JoystickEventState(SDL_ENABLE);

	cube_set(car->body.bounds,car->body.position.x,car->body.position.y,car->body.position.z,2,2,2);

	cube_set(start->body.bounds,-40,-10,0,80,20,1);

	cube_set(check1->body.bounds,-310,350,0,20,80,1);

	cube_set(check2->body.bounds,-640,-10,0,80,20,1);

	cube_set(check3->body.bounds,-310,-400,0,20,80,1);



    while (bGameLoopRunning)
    {
		last_time = current_time;
		current_time = SDL_GetTicks();

		car->acceleration.y = 0;

		rotation_impulse = 0;

		brake = 0;

		camera_reverse = 0;

		SDL_PumpEvents();

		while(SDL_PollEvent(&e))
		{
		
			if (e.type == SDL_QUIT)bGameLoopRunning = 0;

			/*
			if (e.type == SDL_JOYBUTTONDOWN)
			{
				if (e.jbutton.button == 12)
				{
					gear += 1;
					if (gear > 2) gear = 2;
				}
				else if (e.jbutton.button == 10)
				{
					gear -= 1;
					if (gear < 0) gear = 0;
				}

			}
			*/

			else if(e.type == SDL_JOYBUTTONDOWN);
				//bGameLoopRunning = 0;
		}


		car->acceleration.y = (SDL_JoystickGetAxis(joy, 5)/32768.0f)+1;

		brake = (SDL_JoystickGetAxis(joy, 4)/32767.0f)+1;

		rotation_impulse = -SDL_JoystickGetAxis(joy, 0)/32767.0f;

		camera_rotation_x = -SDL_JoystickGetAxis(joy, 2)/32767.0f;

		if (rotation_impulse < 0.1 && rotation_impulse > -0.1) rotation_impulse = 0;
		else if (rotation_impulse > 0.1) rotation_impulse -= 0.1;
		else if (rotation_impulse < -0.1) rotation_impulse += 0.1;

		if (camera_rotation_x < 0.1 && camera_rotation_x > -0.1) camera_rotation_x = 0;
		else if (camera_rotation_x > 0.1) camera_rotation_x -= 0.1;
		else if (camera_rotation_x < -0.1) camera_rotation_x += 0.1;

		if (SDL_JoystickGetButton(joy,7) == 1) camera_reverse = 1;
		
		if(SDL_JoystickGetButton(joy, 12) == 1 || key_state[SDL_SCANCODE_D])
		{
			if(square == 0)
			{
				gear += 1;
				if (gear > 2) gear = 2;
				square = 1;
			}
		}
		else
		{
			square = 0;
		}

		if(SDL_JoystickGetButton(joy, 10) == 1 || key_state[SDL_SCANCODE_A])
		{
			if(cross == 0)
			{
				gear -= 1;
				if (gear < 0) gear = 0;
				cross = 1;
			}
		}
		else
		{
			cross = 0;
		}
	
		
		 //slog("(%d)",SDL_JoystickGetAxis(joy, 0));
		 //slog("(%d)",SDL_JoystickGetAxis(joy, 1));
		 //slog("(%d)",SDL_JoystickGetAxis(joy, 2));
		 //slog("(%d)",SDL_JoystickGetAxis(joy, 3));
		 //slog("(%d)",SDL_JoystickGetAxis(joy, 4));
		 //slog("(%d)",SDL_JoystickGetAxis(joy, 5));

		//slog("(%d)",SDL_JoystickGetButton(joy, 1));

         if (key_state[SDL_SCANCODE_ESCAPE])
         {
			 bGameLoopRunning = 0;
		 }

		 if (key_state[SDL_SCANCODE_W])
		 {
			 car->acceleration.y = 2;
		 }
		 if (key_state[SDL_SCANCODE_LEFT])
		 {
			 rotation_impulse = 1;
		 }
		 if (key_state[SDL_SCANCODE_RIGHT])
		 {
			 rotation_impulse = -1;
		 }

		  if (key_state[SDL_SCANCODE_SPACE])
		 {
			 brake = 1;
		 }

		  if(gear == 1) 
		  {
			  car->body.velocity.y += car->acceleration.y*5*(current_time - last_time)/1000.0f;
			  if(car->acceleration.y > 0.1)car->body.velocity.y *= 0.972;
			  else{car->body.velocity.y *= 0.98;}
		  }

		  else if(gear == 2) 
		  {
			  car->body.velocity.y += car->acceleration.y*1*(current_time - last_time)/1000.0f;
			  if(car->acceleration.y > 0.1)car->body.velocity.y *= 0.997;
			  else{car->body.velocity.y *= 0.98;}
		  }

		  else if(gear == 0) 
		  {
			  car->body.velocity.y -= car->acceleration.y*1*(current_time - last_time)/1000.0f;
			  car->body.velocity.y *= 0.98;
		  }

		if (car->body.velocity.y > 0)
		{
			car->body.velocity.y -= brake*4*(current_time - last_time)/1000.0f;
			if  (car->body.velocity.y < 0) car->body.velocity.y = 0;
		}
		else if (car->body.velocity.y < 0)
		{
			car->body.velocity.y += brake*4*(current_time - last_time)/1000.0f;
			if  (car->body.velocity.y > 0) car->body.velocity.y = 0;
		}
		else {car->body.velocity.y = 0;}
		
		cube_set(car->body.bounds,car->body.position.x,car->body.position.y,car->body.position.z,2,2,2);

		if (cube_cube_intersection(car->body.bounds,start->body.bounds) == 1 && checkpoint == 3) 
		{
				checkpoint = 0;
				lap++;
				slog("(%d)",checkpoint);
				slog("(%d)",lap);
		}

		if (cube_cube_intersection(car->body.bounds,check1->body.bounds) == 1 && checkpoint == 0) 
		{
				checkpoint = 1;
				slog("(%d)",checkpoint);
		}

		if (cube_cube_intersection(car->body.bounds,check2->body.bounds) == 1 && checkpoint == 1) 
		{
				checkpoint = 2;
				slog("(%d)",checkpoint);
		}

		if (cube_cube_intersection(car->body.bounds,check3->body.bounds) == 1 && checkpoint == 2) 
		{
				checkpoint = 3;
				slog("(%d)",checkpoint);
		}






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
        }

		/*
		slog("(%f)",car->body.velocity.y);
		
        slog("(%f,%f,%f)",car->rotation.x,car->rotation.y,car->rotation.z);

		slog("(%f,%f,%f)",car->body.position.x,car->body.position.y,car->body.position.z);
		*/
		cameraRotation.z -= rotation_impulse*3;

		cameraRotation.y = -rotation_impulse*1.5;

		cameraRotation.z += camera_rotation_x*100;

		if(camera_reverse == 1) cameraRotation.z += 180;

		vec3d_add(
            cameraPosition,
            cameraPosition,
            vec_scale(vec3d(sin(cameraRotation.z * DEGTORAD),-cos(cameraRotation.z * DEGTORAD),0), 7.5 + (car->body.velocity.y*0.5))
		);

		cameraPosition.z = 5;

		cameraRotation.x = 70;

        graphics3d_frame_begin();

        glPushMatrix();

		draw_title();  //used for the blue background

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

		entity_draw(start);

		//entity_draw(check1);

		//entity_draw(check2);

		//entity_draw(check3);

		entity_draw(car);

		glPopMatrix();

		draw_speed();

		draw_gear();

		draw_lap();

        glPopMatrix();

        /* drawing code above here! */

        graphics3d_next_frame();

		//cameraPosition.z = 0;

		cameraRotation.x = 90;

		cameraPosition = car->body.position;
    } 

	// Close if opened
    if (SDL_JoystickGetAttached(joy)) {
        SDL_JoystickClose(joy);
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