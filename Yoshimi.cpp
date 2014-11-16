#include "Yoshimi.h"
#include "GameApplication.h"
#define _USE_MATH_DEFINES   
#include <math.h>

Yoshimi::Yoshimi(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, GameApplication* a):
	Agent(SceneManager, name, filename, height, scale, a)
{
	Ogre::Vector3 temp(mBodyNode->getPosition());
	Ogre::Vector3 temp1(temp[0], temp[1]+5, temp[2]+10);//move camera to location. temp[1] is height relative to yoshimi temp[2] is z distance
	Ogre::Camera* cam = app->getCamera();
	cam->pitch(Ogre::Radian(10 * M_PI /180));
	cam->setPosition(temp1);
	mBodyNode->attachObject(app->getCamera());	// main character has camera attached ! ?
	fForward = fBackward = fRight = fLeft = false;  //starts by not moving

	doingStuff = false;  //starts not doing anything
	
	mBodyNode->showBoundingBox(true);  //for testing purposes
	//Get them animations brah
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

void Yoshimi::update(Ogre::Real deltaTime){
	this->updateAnimations(deltaTime);	// Update animation playback
	this->updateLocomote(deltaTime);	// Update Locomotion
}

void Yoshimi::updateLocomote(Ogre::Real deltaTime){
	Ogre::Quaternion q;
	double speed = 0.2;  //how fast is Yoshimi?
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

	//Set Yoshimi Animation based on movement (if not already doing stuff)
	if (!doingStuff){
		if (!fForward && !fLeft && !fRight && !fBackward){
			if (yoshAnim != IDLE_THREE) setAnimation(IDLE_THREE);
		}else{
			if (yoshAnim != STEALTH) setAnimation(STEALTH);
		}
	}
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
	mBodyNode->yaw(Ogre::Degree(arg.state.X.rel * -0.5f));
    //mBodyNode->pitch(Ogre::Degree(arg.state.Y.rel * -0.1f));
}

void Yoshimi::updateAnimations(Ogre::Real deltaTime){

	//If Yoshimi has an active animation, call the update method
	if (yoshAnim != ANIM_NONE){
		mAnims[yoshAnim]->addTime(deltaTime * 2);
		if (mAnims[yoshAnim]->hasEnded()) doingStuff = false;   //no longer doing stuff
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
	else if (key == 't') setAnimation(ATTACK_ONE, true);  //throw fishbomb
	else if (key == 's') setAnimation(ATTACK_THREE, true);//use sword
	else if (key == 'k') setAnimation(KICK, true);		  //judo-kick
}