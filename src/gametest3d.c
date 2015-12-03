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
 Vec3D cameraPosition = {0,-10,0};
 Vec3D cameraRotation = {90,0,0};

 void resetCamera(Vec3D targetPosition, Vec3D targetRotation)
 {
	//Vec3D forward = Normalize();
	cameraRotation = targetRotation;
	
 }

 
 
 int main(int argc, char *argv[])
{
    GLuint vao;
    float r = 0;
    GLuint triangleBufferObject;
    char bGameLoopRunning = 1;
   

	Vec3D carPosition = {0,0,0};
    Vec3D carRotation = {0,0,0};

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

	Entity* car;

	entity_init(255);

	car = entity_new();

    
    init_logger("gametest3d.log");
    if (graphics3d_init(1024,768,1,"gametest3d",33) != 0)
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
    
    //obj = obj_load("models/cube.obj");
    //texture = LoadSprite("models/cube_text.png",1024,1024);

	entity_obj_load(car,"models/cube.obj");
	entity_load_sprite(car,"models/cube_text.png",1024,1024);

    bgobj = obj_load("models/mountainvillage.obj");
    bgtext = LoadSprite("models/mountain_text.png",1024,1024);
    
    while (bGameLoopRunning)
    {
        while ( SDL_PollEvent(&e) ) 
        {
            if (e.type == SDL_QUIT)
                bGameLoopRunning = 0;
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    bGameLoopRunning = 0;
                }



                /*else if (e.key.keysym.sym == SDLK_SPACE)
                {
                    cameraPosition.z++,
                    slog("move up");
                    slog("(%f,%f,%f)",cameraPosition.x,cameraPosition.y,cameraPosition.z);
                }
                else if (e.key.keysym.sym == SDLK_z)
                {
                    cameraPosition.z--,
                    slog("move down");
                    slog("(%f,%f,%f)",cameraPosition.x,cameraPosition.y,cameraPosition.z);
                }
                else if (e.key.keysym.sym == SDLK_w)
                {
                    vec3d_add(
                        cameraPosition,
                        cameraPosition,
                        vec3d(
                            -sin(cameraRotation.z * DEGTORAD),
                            cos(cameraRotation.z * DEGTORAD),
                            0
                        ));
                    slog("move forward");
                    slog("(%f,%f,%f)",cameraPosition.x,cameraPosition.y,cameraPosition.z);
                }
                else if (e.key.keysym.sym == SDLK_s)
                {
                    vec3d_add(
                        cameraPosition,
                        cameraPosition,
                        vec3d(
                            sin(cameraRotation.z * DEGTORAD),
                            -cos(cameraRotation.z * DEGTORAD),
                            0
                        ));
                    slog("move back");
                    slog("(%f,%f,%f)",cameraPosition.x,cameraPosition.y,cameraPosition.z);
                }
                else if (e.key.keysym.sym == SDLK_d)
                {
                    vec3d_add(
                        cameraPosition,
                        cameraPosition,
                        vec3d(
                            cos(cameraRotation.z * DEGTORAD),
                            sin(cameraRotation.z * DEGTORAD),
                            0
                        ));
                    slog("move right");
                    slog("(%f,%f,%f)",cameraPosition.x,cameraPosition.y,cameraPosition.z);
                }
                else if (e.key.keysym.sym == SDLK_a)
                {
                    vec3d_add(
                        cameraPosition,
                        cameraPosition,
                        vec3d(
                            -cos(cameraRotation.z * DEGTORAD),
                            -sin(cameraRotation.z * DEGTORAD),
                            0
                        ));
                    slog("move left");
                    slog("(%f,%f,%f)",cameraPosition.x,cameraPosition.y,cameraPosition.z);
                }
                else if (e.key.keysym.sym == SDLK_LEFT)
                {
                    cameraRotation.z += 1;
                    slog("rotate left");
                    slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
                }
                else if (e.key.keysym.sym == SDLK_RIGHT)
                {
                    cameraRotation.z -= 1;
                    slog("right");
                    slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
                }
                else if (e.key.keysym.sym == SDLK_UP)
                {
                    cameraRotation.x += 1;
                    slog("up");
                    slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
                }
                else if (e.key.keysym.sym == SDLK_DOWN)
                {
                    slog("down");
                    cameraRotation.x -= 1;
                    slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
                }
				else if (e.key.keysym.sym == SDLK_COMMA)
                {
                    slog("deccel");
                    needleRotation -= 1;
					if(needleRotation < 0)needleRotation=0;
                    //slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
                }
				else if (e.key.keysym.sym == SDLK_PERIOD)
                {
                    slog("accel");
                    needleRotation += 1;
					if(needleRotation > 270)needleRotation=270;
                    //slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
                }*/

				
				 else if (e.key.keysym.sym == SDLK_w)
                {
                    carPosition.y += 1;
					cameraPosition.y +=1;
                    slog("move forward");
                }
                else if (e.key.keysym.sym == SDLK_s)
                {
                    carPosition.y -= 1;
					cameraPosition.y -=1;
                    slog("move back");
                }
				  else if (e.key.keysym.sym == SDLK_LEFT)
                {
                    cameraRotation.z += 1;
                    slog("rotate left");
                    slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
                }
                else if (e.key.keysym.sym == SDLK_RIGHT)
                {
                    cameraRotation.z -= 1;
                    slog("right");
                    slog("(%f,%f,%f)",cameraRotation.x,cameraRotation.y,cameraRotation.z);
                }


				/*
                else if (e.key.keysym.sym == SDLK_d)
                {
                    carPosition.x += 1;
                    slog("move right");
                }
                else if (e.key.keysym.sym == SDLK_a)
                {
                    carPosition.x -= 1;
                    slog("move left");
                }
				*/

				
				/*
				else if (e.key.keysym.sym == SDLK_LEFT)
                {
                    carRotation.z += 1;
					//resetCamera(carPosition, carRotation);
                    slog("rotate left");
                }
                else if (e.key.keysym.sym == SDLK_RIGHT)
                {
                    carRotation.z -= 1;
                    slog("rotate right");
                }*/


            }
        }

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
            vec3d(5,5,5),
            vec4d(1,1,1,1),
            bgtext
        ); 
        
		/*obj_draw(
            obj,
            carPosition,
            carRotation,
            vec3d(0.5,0.5,0.5),
            vec4d(1,1,1,1),
            texture
        );*/
        //if (r > 360)r -= 360;

		entity_draw(car);

		glPopMatrix();

		draw_speed();

        glPopMatrix();

        /* drawing code above here! */

        graphics3d_next_frame();
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