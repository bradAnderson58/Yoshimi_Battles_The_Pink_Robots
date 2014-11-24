#include "Yoshimi.h"
#include "GameApplication.h"
#define _USE_MATH_DEFINES   
#include <math.h>

Yoshimi::Yoshimi(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, GameApplication* a):
	Agent(SceneManager, name, filename, height, scale, a)
{
	//origin
	mBodyNode->setPosition(0.0,0.0,0.0);
	mBodyNode->yaw(Ogre::Radian(M_PI));

	//Camera set up here
	Ogre::Vector3 temp(mBodyNode->getPosition());
	Ogre::Vector3 temp1(temp[0], temp[1]+10, temp[2]+10);//move camera to location. temp[1] is height relative to yoshimi temp[2] is z distance
	Ogre::Camera* cam = app->getCamera();
	cam->pitch(Ogre::Radian(10 * M_PI /180));
	cam->setPosition(temp1);

	//attaching camera code here
	Ogre::SceneNode* camNode = mBodyNode->createChildSceneNode();
	camNode->attachObject(cam);

	fForward = fBackward = fRight = fLeft = false;  //starts by not moving
	doingStuff = false;  //starts not doing anything
	speed = 2;	//Yoshimi a quick one 

	//mBodyNode->showBoundingBox(true);										//for testing purposes Yoshimi box

	//attack space
	mAttackNode = mBodyNode->createChildSceneNode();
	mAttackEntity = mSceneMgr->createEntity("attackCube", Ogre::SceneManager::PT_CUBE);
	mAttackEntity->setMaterialName("Examples/RustySteel");
	mAttackNode->attachObject(mAttackEntity);
	
	//set up the position of the attackNode
	mAttackNode->setPosition(0.0f ,100.0f, -100.0f);
	mAttackNode->setVisible(false);
	//mAttackNode->showBoundingBox(true);											//for testing  Attack box
	
	//use this to check animations if needed
	/*Ogre::AnimationStateSet* aSet = mBodyEntity->getAllAnimationStates();
	Ogre::AnimationStateIterator iter = mBodyEntity->getAllAnimationStates()->getAnimationStateIterator();
	while (iter.hasMoreElements())
	{
		Ogre::AnimationState *a = iter.getNext();
		std::string s = a->getAnimationName();
		std::cout << s << std::endl;
	}*/
	
	setupAnimations();
}

Yoshimi::~Yoshimi(void)
{
	
}

void Yoshimi::update(Ogre::Real deltaTime){
	this->updateAnimations(deltaTime);	// Update animation playback
	this->updateLocomote(deltaTime);	// Update Locomotion
	this->collisionRobots();

}

void Yoshimi::updateLocomote(Ogre::Real deltaTime){
	Ogre::Quaternion q;
	double vel = 0.2;
	Ogre::Vector3 translator = Ogre::Vector3::ZERO;

	//use direction for forward and backward
	mDirection = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z;
	this->collisionWalls();
	//90 degrees from mDirection for right and left
	q.FromAngleAxis(Ogre::Radian(M_PI) / 2, Ogre::Vector3(0,1,0));
	Ogre::Vector3 side = q*mDirection;

	//set translation based on which keys are currentl
	if (!doingStuff || yoshAnim == JUMP){
		if (fForward) translator += (mDirection * -vel);
		if (fLeft) translator += (side * -vel);
		if (fRight) translator += (side * vel);
		if (fBackward) translator += (mDirection * vel);
	}
	//Set Yoshimi Animation based on movement (if not already doing stuff)
	if (!doingStuff){
		if (!fForward && !fLeft && !fRight && !fBackward){
			if (yoshAnim != IDLE_THREE) setAnimation(IDLE_THREE);
		}else{
			if (yoshAnim != STEALTH) setAnimation(STEALTH);
		}
	}

	float maxX = app->getXmax() * 10 -5;
	float maxZ = app->getZmax() * 10 -5;

	Ogre::Vector3 newPos = translator + getPosition();

	if (newPos[0] > maxX -.5){
		newPos[0] = maxX -.5;
	}
	else if (newPos[0] < -maxX + .5){
		newPos[0] = -maxX + .5;
	}
	if (newPos[2] > maxZ - .5){
		newPos[2] = maxZ - .5;
	}
	else if(newPos[2] < -maxZ + .5){
		newPos[2] = -maxZ + .5;
	}

	Ogre::Vector3 hp = app->getHousePointer()->getPosition();
	hp[1] = 0;
	if (newPos.distance(hp) < 25){
		float xvals = newPos[0] - hp[0];
		float zvals = newPos[2] - hp[2];
		if ((xvals < 15 && xvals > -18) && (zvals < 18 && zvals > -15)){
			if (abs(xvals) > abs(zvals)){
				if(xvals > 0){
					newPos[0] = hp[0] + 15;
				}
				else{
					newPos[0] = hp[0] - 18;
				}
			}
			else{
				if (zvals > 0){
					newPos[2] = hp[2] + 18;
				}
				else{
					newPos[2] = hp[2] - 15;
				}
			}
		}
	}
	mBodyNode->setPosition(newPos);
}

//Set movement flags based on a char to represent direction and boolean on or not
void Yoshimi::setMovement(char dir, bool on){
	
	//Set the correct flag to the new boolean value
	if (dir == 'f') fForward = on;
	else if (dir == 'b') fBackward = on;
	else if (dir == 'r') fRight = on;
	else if (dir == 'l') fLeft = on;
}

void Yoshimi::rotationCode(OIS::MouseEvent arg){
	mBodyNode->yaw(Ogre::Degree(arg.state.X.rel * -0.5f));
    //mBodyNode->pitch(Ogre::Degree(arg.state.Y.rel * -0.1f));
}

void Yoshimi::updateAnimations(Ogre::Real deltaTime){

	//If Yoshimi has an active animation, call the update method
	if (yoshAnim != ANIM_NONE){
		mAnims[yoshAnim]->addTime(deltaTime * speed);
		if (mAnims[yoshAnim]->hasEnded()){
			doingStuff = false;   //no longer doing stuff
			speed = 2;
		}
	}
	//transitions
	fadeAnimations(deltaTime);
	
}

void Yoshimi::fadeAnimations(Ogre::Real deltaTime){
	using namespace Ogre;

	for (int i = 0; i < 20; i++)
	{
		if (mFadingIn[i])
		{
			// slowly fade this animation in until it has full weight
			Real newWeight = mAnims[i]->getWeight() + deltaTime * 7.5f; //ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight >= 1) mFadingIn[i] = false;
		}
		else if (mFadingOut[i])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			Real newWeight = mAnims[i]->getWeight() - deltaTime * 7.5f; //ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}

void Yoshimi::setupAnimations(){

	
	this->mTimer = 0;	// Start from the beginning
	this->mVerticalVelocity = 0;	// Not jumping

	// this is very important due to the nature of the exported animations
	mBodyEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	// Name of the animations for this character
	Ogre::String animNames[] =
		{  "Attack1", "Attack2", "Attack3", "Backflip", "Block", "Climb", "Crouch", "Death1", "Death2", "HighJump", "Idle1", "Idle2",
		"Idle3", "Jump", "JumpNoHeight", "Kick", "SideKick", "Spin", "Stealth", "Walk"};

	// populate our animation list
	for (int i = 0; i < 20; i++)
	{
		mAnims[i] = mBodyEntity->getAnimationState(animNames[i]);
		
		//Some animations are not looping
		if (animNames[i] == "Idle3" || animNames[i] == "Stealth") mAnims[i]->setLoop(true);
		else mAnims[i]->setLoop(false);

		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setAnimation(IDLE_TWO);

}

void Yoshimi::setAnimation(AnimID id, bool reset){
	if (yoshAnim >= 0 && yoshAnim < 20)
	{
		// if we have an old animation, fade it out
		mFadingIn[yoshAnim] = false;
		mFadingOut[yoshAnim] = true;
	}

	yoshAnim = id; 

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void Yoshimi::buttonAnimation(char key){
	if (key == 'j'){
		setAnimation(JUMP, true);
	}
	else if (key == 't') setAnimation(ATTACK_ONE, true);  //throw fishbomb?
	else if (key == 's') setAnimation(ATTACK_THREE, true);//use sword
	else if (key == 'k') setAnimation(KICK, true);		  //judo-kick
}

//Yoshimi checks the robot list to see if any robots are close enough to hit
//If so, they get hurted and such
//TODO: Optimize so that Yoshimi only has to check a certain amount of robots?  Octree?
void Yoshimi::checkHits(char attack){
	
	//get bounding boxes for Yoshimi and her adversaries
	Ogre::AxisAlignedBox aRange = mAttackEntity->getWorldBoundingBox();
	Ogre::AxisAlignedBox rRange;

	for (Robot *robot : app->getRobotList()){
		rRange = robot->getBoundingBox();

		if (aRange.intersects(rRange)){
			robot->getHit(attack, mDirection);	//hit em
		}
	}

}

void Yoshimi::collisionRobots(){
	std::list<Robot*> robots = app->getRobotList();

	Ogre::Vector3 temp(0,0,0);
	Ogre::Vector3 pos = mBodyNode->getPosition();
	Ogre::Vector3 rPos;
	for(Robot* r : robots){
		rPos = r->getPosition();
		if (pos.distance(rPos) < 2){
			temp = pos - rPos;
			temp.normalise();
			temp = rPos + (temp * 2);
			temp[1] = 0;
			setPosition(temp[0], 0, temp[2]);
		}
	}
}

void Yoshimi::collisionWalls(){
	
}