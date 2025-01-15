#pragma once
#include "CharacterBase.h"

class Player : public CharacterBase
{
public:

	Player(BaseScene* baseScene_) : CharacterBase(baseScene_){}

	void Initialize() override;

	void Update() override;

	void Draw() override;



};

