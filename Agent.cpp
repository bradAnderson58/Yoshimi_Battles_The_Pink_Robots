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
	mBodyNode->yaw(Ogre::Degree(-90)); //fish goes this way

	//AnimationStateSet* aSet = mBodyEntity->getAllAnimationStates();
	//AnimationStateIterator iter = mBodyEntity->getAllAnimationStates()->getAnimationStateIterator();
	//while (iter.hasMoreElements())
	//{
	//	AnimationState *a = iter.getNext();
	//	std::string s = a->getAnimationName();
	//}

	//setupAnimations();  // load the animation for this character

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
	if (projectile) // Lecture 12
		shoot(deltaTime);
	else
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

void
Agent::fire(int trajectory, int velocity) // lecture 12
{

	//At the end of the sequence, the fish will return to the starting position / orientation
	defaultPos = this->mBodyNode->getPosition();
	defaultOrient = this->mBodyNode->getOrientation();

	projectile = true; // turns on the movement
	this->setBaseAnimation(ANIM_NONE);  //Swimming?

	// set up the initial state
	initPos = this->mBodyNode->getPosition();
	vel.x = 0;											 //x value not changed (possibly add later)
	vel.y = sin((M_PI * trajectory) / 180) * velocity;   //y and z values are determined by the angle of trajectory multiplied by velocity
	vel.z = -cos((M_PI * trajectory) / 180) * velocity;

	//gravity
	gravity.x = 0;
	gravity.y = -9.81;
	gravity.z = 0;
	//ps->setVisible(true);

	//view the fish bounding box by uncommenting this:
	//this->mBodyNode->showBoundingBox(true); 
	this->mBodyNode->roll(Ogre::Degree(-vel.y));  //point nose towards vector of movement
}

void
Agent::shoot(Ogre::Real deltaTime) // lecture 12 call for every frame of the animation
{
	using namespace Ogre;

	Vector3 pos = this->mBodyNode->getPosition();
	vel = vel + (gravity * deltaTime);
	pos = pos + (vel * deltaTime); // velocity
	pos = pos + 0.5 * gravity * deltaTime * deltaTime; // acceleration

	this->mBodyNode->setPosition(pos);

	//update the orientation to 'follow' the arc
	this->mBodyNode->setOrientation(defaultOrient);
	this->mBodyNode->roll(Ogre::Degree(-vel.y));

	//Collision code
	Ogre::AxisAlignedBox objBox = this->mBodyEntity->getWorldBoundingBox();
	bool hit = objBox.intersects(app->getBox());

	if (this->mBodyNode->getPosition().y <= 0 || hit) // if it get close to the ground, OR hits barrel stop
	{
		//if the barrel was hit, update the success gui
		if (hit) {
			successes++;
			app->setSuccess(successes);
			app->changeLighting(GameApplication::GREEN);
		}
		else app->changeLighting(GameApplication::RED);  //this is for a miss

		// when finished reset
		projectile = false;
		setBaseAnimation(ANIM_NONE);  //stop swimming
		//ps->setVisible(false);

		this->mBodyNode->setPosition(initPos); //Go back to try again! 

		//Go back to trajectory setting in slider
		app->setState(GameApplication::TRAJECTORY);
		app->slideCaption("Trajectory");

		//reset fish to original positioning
		this->mBodyNode->setPosition(defaultPos);
		this->mBodyNode->setOrientation(defaultOrient);
		

	}
}

