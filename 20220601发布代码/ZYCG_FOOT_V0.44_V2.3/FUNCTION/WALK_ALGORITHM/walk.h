#ifndef __WALK_H
#define __WALK_H
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	bool isInit;
	void (*Init)(uint32_t);
	void(*Algorithm_Walk)(short*, short*);
	uint8_t(*GetWalk)(void);
	float(*GetSpeed)(void);
}VoidGear_Walk_t;

extern VoidGear_Walk_t walk;
VoidGear_Walk_t* WalkGetObj(void);

#endif
