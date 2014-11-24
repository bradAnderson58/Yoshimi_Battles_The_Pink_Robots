#include "Agent.h"
#include "GameApplication.h"

#define _USE_MATH_DEFINES   //for some reason
#include <math.h>

Agent::Agent(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, GameApplication* a)
{
	using namespace Ogre;

	mSceneMgr = SceneManager; // keep a pointer to where this agent will be

	if (mSceneMgr == NULL)
	{
		std::cout << "ERROR: No valid scene manager in Agent constructor" << std::endl;
		return;
	}

	this->height = height;
	this->scale = scale;
	this->app = a;  //this is the game app

	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, filename); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node

	mBodyNode->translate(0,height,0); // make the Ogre stand on the plane (almost)
	mBodyNode->scale(scale,scale,scale); // Scale the figure
	//mBodyNode->yaw(Ogre::Degree(-90)); //fish goes this way


	//setupAnimations();  // load the animation for this character
	initPos = mBodyNode->getPosition();
	projectile = false; // lecture 12
	ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
	//ps = mSceneMgr->createParticleSystem("Fountain1", "Examples/PurpleFountain");
	Ogre::SceneNode* mnode = mBodyNode->createChildSceneNode();
	mnode->roll(Degree(180));
	//mnode->attachObject(ps);
	//ps->setVisible(false);

	// configure walking parameters
	mWalkSpeed = 35.0f;	
	mDirection = Ogre::Vector3::ZERO;

	successes = 0;  //no successes yet!
}

Agent::~Agent(){
	// mSceneMgr->destroySceneNode(mBodyNode); // Note that OGRE does not recommend doing this. It prefers to use clear scene
	// mSceneMgr->destroyEntity(mBodyEntity);
}

void 
Agent::setPosition(float x, float y, float z)
{
	this->mBodyNode->setPosition(x, y + height, z);
}

// update is called at every frame from GameApplication::addTime
void
Agent::update(Ogre::Real deltaTime) 
{
	this->updateLocomote(deltaTime);	// Update Locomotion
	
	this->updateAnimations(deltaTime);	// Update animation playback
}


void 
Agent::setupAnimations()
{
	
}

void 
Agent::setBaseAnimation(AnimID id, bool reset)
{
	
}
	
void Agent::setTopAnimation(AnimID id, bool reset)
{
	
}

void 
Agent::updateAnimations(Ogre::Real deltaTime)
{
	
}

void 
Agent::fadeAnimations(Ogre::Real deltaTime)
{
	
}

bool 
Agent::nextLocation()
{
	return true;
}

void 
Agent::updateLocomote(Ogre::Real deltaTime)
{
}


