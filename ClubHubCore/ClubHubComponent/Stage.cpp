#include "Stage.h"
#include "Entity.h"
#include "Components\CameraComponent.h"
#include "Renderer\Renderable.h"
#include "Window\Graphics.h"
#include "Game.h"
#include "Components\ColliderComponent.h"

Stage::Stage()
{
	currentEntity = 0;
	currentQueued = 0;
	currentRender = 0;
}
Stage::~Stage()
{
	for( int i = 0; i < currentEntity; i++ )
	{
		delete entities[i];
	}
}

Game* Stage::getGame()
{
	return game;
}

void Stage::addEntity( Entity * entity )
{
	entities[currentEntity++] = entity;
	entity->stage = this;
}

bool Stage::hasEntity( Entity * entity )
{
	bool found = false;
	int foundIndex = 0;
	for( int i = 0; !found && i < currentEntity; i++ )
	{
		if( entity == entities[i] )
		{
			foundIndex = i;
			found = true;
		}
	}
	return found;
}

void Stage::deleteEntity( Entity * entity )
{
	queuedRemovals[ currentQueued++ ] = entity;
}

void Stage::clear()
{
	for( int i = 0; i < currentEntity; i++ )
	{
		queuedRemovals[ currentQueued++ ] = entities[i];
	}
}

void Stage::update( float dt )
{
	for( int i = 0; i < currentEntity; i++ )
	{
		if( !entities[i]->inited )
		{
			for( int c = 0; c < entities[i]->components->count(); c++ )
			{
				entities[i]->components->getInserted( c )->init();
			}
			entities[i]->inited = true;
		}

		for( int c = 0; c < entities[i]->components->count(); c++ )
		{
			entities[i]->components->getInserted( c )->update( dt );
		}
	}

	ColliderComponent *a, *b;
	glm::vec3 collNorm;
	float inter;

	for( int i = 0; i < currentEntity-1; i++ )
	{
		a = entities[i]->getComponent<ColliderComponent>();
		if( a != nullptr )
		{
			for( int j = i+1; j < currentEntity; j++ )
			{
				b = entities[j]->getComponent<ColliderComponent>();
				if( b != nullptr && entities[i]->inited && entities[j]->inited && a->collidesWith(b, &collNorm, &inter ) )
				{
					for( int c = 0; c < entities[i]->components->count(); c++ )
					{
						entities[i]->components->getInserted( c )->onCollide( b->parent, collNorm, inter );
					}
					for( int c = 0; c < entities[j]->components->count(); c++ )
					{
						entities[j]->components->getInserted( c )->onCollide( a->parent, -collNorm, inter );
					}
				}
			} 
		}
	}


	for( int j = 0; j < currentQueued; j++ )
	{
		bool found = false;
		int foundIndex = 0;
		for( int i = 0; !found && i < currentEntity; i++ )
		{
			if( queuedRemovals[j] == entities[i] )
			{
				foundIndex = i;
				found = true;
			}
		}

		if( found )
		{
			currentEntity--;
			delete entities[ foundIndex ];
			entities[ foundIndex ] = entities[ currentEntity ];
		}
	}
	currentQueued = 0;
}

void Stage::subscribeRender( CameraComponent* camera )
{
	renders[currentRender++] = camera;
}

void Stage::draw()
{
	Graphics * g = game->getGraphicsHandle();

	g->clear();

	for( int j = 0; j < currentRender; j++ )
	{
		if( hasEntity( renders[j]->getParent() ) )
		{
			renders[j]->setUp();
			for( int run = 0; run < renders[j]->getNumberOfRuns(); run++ )
			{
				renders[j]->startRun( run );

				for( int i = 0; i < currentEntity; i++ )
				{
					/*for( auto iter = entities[i]->components->begin(); iter != entities[i]->components->end(); iter++ )
					{
						iter->second->draw();
					}*/
					//if( entities[ i ]->inited )
					{
						for( int c = 0; c < entities[i]->components->count(); c++ )
						{
							entities[i]->components->getInserted( c )->draw();
						}
					}
				}

				renders[j]->endRun( run );
			}
			renders[j]->breakDown();


			/*for( int i = 0; i < currentEntity; i++ )
			{
				entities[i]->draw();
			}*/
		}
	}
	currentRender = 0;
}