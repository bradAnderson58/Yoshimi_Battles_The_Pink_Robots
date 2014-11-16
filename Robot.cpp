#include "Robot.h"
#include "GameApplication.h"
#define _USE_MATH_DEFINES   
#include <math.h>

Robot::Robot(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, GameApplication* a):
	Agent(SceneManager, name, filename, height, scale, a)
{
	Ogre::AnimationStateSet* aSet = mBodyEntity->getAllAnimationStates();
	Ogre::AnimationStateIterator iter = mBodyEntity->getAllAnimationStates()->getAnimationStateIterator();
	while (iter.hasMoreElements())
	{
		Ogre::AnimationState *a = iter.getNext();
	}
	counter = 0;
	testing = 0;
	setupAnimations();
}

void Robot::update(Ogre::Real deltaTime){
	this->updateAnimations(deltaTime);	// Update animation playback
	this->updateLocomote(deltaTime);	// Update Locomotion
	/*counter++;
	if (counter > 100){
		counter = counter % 100;
		testing++;
		if (testing > 5){
			testing = 0;
		}
		if(testing == 0){
			setAnimation(DIE);
		}
		if(testing == 1){
			setAnimation(IDLE);
		}
		if(testing == 2){
			setAnimation(SHOOT);
		}
		if(testing == 3){
			setAnimation(SLUMP);
		}
		if(testing == 4){
			setAnimation(WALK);
		}
		if(testing == 5){
			setAnimation(ANIM_NONE);
		}
	}*/
}

void Robot::updateLocomote(Ogre::Real deltaTime){
	Ogre::Quaternion q;
	double speed = 0.5;  //how fast is Robot?
	Ogre::Vector3 translator = Ogre::Vector3::ZERO;

	//use direction for forward and backward
	mDirection = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z;

	//90 degrees from mDirection for right and left
	q.FromAngleAxis(Ogre::Radian(M_PI) / 2, Ogre::Vector3(0,1,0));
	Ogre::Vector3 side = q*mDirection;

	mBodyNode->translate(translator);
}

//Set movement flags based on a char to represent direction and boolean on or not
void Robot::setMovement(char dir, bool on){
	
}

void Robot::rotationCode(OIS::MouseEvent arg){
	mBodyNode->yaw(Ogre::Degree(arg.state.X.rel * -0.1f));
}

void Robot::updateAnimations(Ogre::Real deltaTime){
	mTimer += deltaTime; // how much time has passed since the last update

	if (robAnim != ANIM_NONE){
		mAnims[robAnim]->addTime(deltaTime * 1);
	}
	//transitions
	//fadeAnimations(deltaTime);
	
}

void Robot::fadeAnimations(Ogre::Real deltaTime){
	using namespace Ogre;

	for (int i = 0; i < 5; i++)
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

void Robot::setupAnimations(){
	this->mTimer = 0;	// Start from the beginning
	this->mVerticalVelocity = 0;	// Not jumping

	// this is very important due to the nature of the exported animations
	mBodyEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	// Name of the animations for this character
	Ogre::String animNames[] =
		{"Die","Idle","Shoot","Slump","Walk"};

	// populate our animation list
	for (int i = 0; i < 5; i++)
	{
		mAnims[i] = mBodyEntity->getAnimationState(animNames[i]);
		if(i == 0 || i==3){
			mAnims[i]->setLoop(false);
		}
		else{
			mAnims[i]->setLoop(true);
		}
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setAnimation(WALK);
}

void Robot::setAnimation(AnimID id, bool reset){
	if (robAnim >= 0 && robAnim < 5)
	{
		// if we have an old animation, fade it out
		mFadingIn[robAnim] = false;
		mFadingOut[robAnim] = true;
	}

	robAnim = id; 

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
	}
}