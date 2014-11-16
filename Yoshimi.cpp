#include "Yoshimi.h"
#include "GameApplication.h"
#define _USE_MATH_DEFINES   
#include <math.h>

Yoshimi::Yoshimi(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, GameApplication* a):
	Agent(SceneManager, name, filename, height, scale, a)
{
	mBodyNode->attachObject(app->getCamera());	// main character has camera attached ! ?
	fForward = fBackward = fRight = fLeft = false;  //starts by not moving
	Ogre::AnimationStateSet* aSet = mBodyEntity->getAllAnimationStates();
	Ogre::AnimationStateIterator iter = mBodyEntity->getAllAnimationStates()->getAnimationStateIterator();
	while (iter.hasMoreElements())
	{
		Ogre::AnimationState *a = iter.getNext();
		std::string s = a->getAnimationName();
		std::cout << s << std::endl;
	}
	
	setupAnimations();
	//yoshAnim = ANIM_NONE;
}

void Yoshimi::update(Ogre::Real deltaTime){
	this->updateAnimations(deltaTime);	// Update animation playback
	this->updateLocomote(deltaTime);	// Update Locomotion
}

void Yoshimi::updateLocomote(Ogre::Real deltaTime){
	Ogre::Quaternion q;
	double speed = 0.5;  //how fast is Yoshimi?
	Ogre::Vector3 translator = Ogre::Vector3::ZERO;

	//use direction for forward and backward
	mDirection = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z;

	//90 degrees from mDirection for right and left
	q.FromAngleAxis(Ogre::Radian(M_PI) / 2, Ogre::Vector3(0,1,0));
	Ogre::Vector3 side = q*mDirection;

	//set translation based on which keys are currentl 
	if (fForward) translator += (mDirection * -speed);
	if (fLeft) translator += (side * -speed);
	if (fRight) translator += (side * speed);
	if (fBackward) translator += (mDirection * speed);

	mBodyNode->translate(translator);
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
	mBodyNode->yaw(Ogre::Degree(arg.state.X.rel * -0.1f));
    //mBodyNode->pitch(Ogre::Degree(arg.state.Y.rel * -0.1f));
}

void Yoshimi::updateAnimations(Ogre::Real deltaTime){
	std::cout << yoshAnim << std::endl;


	mTimer += deltaTime; // how much time has passed since the last update

	if (yoshAnim != ANIM_IDLE_TOP)
	if (yoshAnim != ANIM_NONE)
	if (mTimer >= mAnims[yoshAnim]->getLength())
		{
			//setTopAnimation(ANIM_IDLE_TOP, true);
			//setBaseAnimation(ANIM_IDLE_BASE, true);
			mTimer = 0;
		}

	if (yoshAnim != ANIM_NONE){
		mAnims[yoshAnim]->addTime(deltaTime * 1);
		std::cout << "Here?" << std::endl;
	}
	//transitions
	//fadeAnimations(deltaTime);
	
}

void Yoshimi::fadeAnimations(Ogre::Real deltaTime){
	using namespace Ogre;

	for (int i = 0; i < 13; i++)
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
		mAnims[i]->setLoop(true);
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setAnimation(IDLE_TWO);

	/*Ogre::AnimationState* thing = mBodyEntity->getAnimationState("Idle2");
	thing->setLoop(true);
	thing->setEnabled(true);*/
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
		//mAnims[id]->setWeight(0);
		//mFadingOut[id] = false;
		//mFadingIn[id] = true;
		//if (reset) mAnims[id]->setTimePosition(0);
	}
}