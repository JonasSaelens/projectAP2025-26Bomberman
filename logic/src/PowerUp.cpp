#include "logic/PowerUp.hpp"

#include "logic/Character.hpp"

namespace logic {

void FirePowerUp::applyTo(Character& character) { character.grantFireBoost(); }

void BombPowerUp::applyTo(Character& character) { character.grantExtraBomb(); }

void SkatesPowerUp::applyTo(Character& character) { character.grantSpeedBoost(); }

} // namespace logic
