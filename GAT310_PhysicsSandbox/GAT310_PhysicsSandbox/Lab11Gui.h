#pragma once
#include "PhysicsGui\GuiBase.h"
#include "glm/glm.hpp"

#include "Particle.h"
#include "ForceGenerator.h"
#include "ParticleWorld.h"

struct VectorGraphic;

class Lab11Gui : public GuiBase
{
	int numParticles;

	VectorGraphic ** particleGraphics;
	ParticleWorld world;
	
	
	ParticleForceGenerator *forceGravity, *forceKeyboard;
	ParticleDrag *forceDrag;
	ParticleSpring **forceSprings;

	void newFrame();
	void vectorGraphicMouseDrag( uint vectorGraphicIndex, const glm::vec3& dragDelta );

	void makeConstraints();

	glm::vec3 vectorFromKeyInput();

	int springCount;

public:
	Lab11Gui();
	~Lab11Gui();

	void reset();
	void resolveCollision( float dt );

	void initialize();
};

