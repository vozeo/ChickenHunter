#include "Weapon.h"

void Weapon::weaponInit(float speed, int attack, int state, int type) {
	setWeaponSpeed(speed);
	setWeaponAttack(attack);
	setWeaponState(state);
	setWeaponType(type);
}