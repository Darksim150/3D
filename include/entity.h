#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "obj.h"
#include "vector.h"
#include "sprite.h"
#include "collisions.h"
#include "body.h"

typedef struct Entity_S
{
    int inuse;
    int uid;    /**<unique id of this entity*/
    char name[128];
    Vec3D acceleration;
    Vec3D rotation;
    Vec3D scale;
    Vec4D color;
    Obj *objModel;
    Obj *objAnimation[24];
    int state;
    float frame;
    Sprite *texture;    /**<object texture*/
    Body body;
    void (*think)(struct Entity_S *self);
}Entity;

/**
 * @brief initialize the entity sub system
 * @param maxEntity the maximum number of simultaneously supported entities.
 */
void entity_init(int maxEntity);

/**
 * @brief get a pointer to a new entity
 * @return NULL on no more entities or error,  a valid entity pointer otherwise
 */
Entity *entity_new();

/**
 * @brief draws all active entities
 */
void entity_draw_all();
void entity_think_all();

/**
 * @brief draws an entity
 * @param ent the entity to draw
 */
void entity_draw(Entity *ent);

/**
 * @brief frees an entity
 */
void entity_free(Entity *ent);

int entity_is_entity(void *data);

void entity_obj_load(Entity *ent, char *filename);

void entity_load_sprite(Entity *ent, char *filename, int fw, int fh);

extern Entity* car;

#endif
