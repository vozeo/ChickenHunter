#include "Weapon.h"

void Weapon::weaponInit(float speed, float attack, int state, int type) {
	setWeaponSpeed(speed);
	setWeaponAttack(attack);
	setWeaponState(state);
	setWeaponType(type);
}