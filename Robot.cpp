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

	//mBodyNode->yaw(Ogre::Degree(180)); //fish goes this way
	//mBodyNode->pitch(Ogre::Degree(180));
	Ogre::AxisAlignedBox uhh = mBodyEntity->getBoundingBox();
	mSceneMgr->showBoundingBoxes(true);
	mBodyNode->showBoundingBox(false);
	counter = 0;
	testing = 0;
	mDirection = Ogre::Vector3(-.05,0,0);//Ogre::Vector3::ZERO;
	setupAnimations();
	int count = mBodyEntity->getNumSubEntities();
	for (int i = 0; i < count; i++){
		mBodyEntity->getSubEntity(i)->getMaterial()->setColourWriteEnabled(true);
		mBodyEntity->getSubEntity(i)->getMaterial()->setDiffuse(1,.6,1,1);
		mBodyEntity->getSubEntity(i)->getMaterial()->setAmbient(1,.6,1);
	}
}

void Robot::update(Ogre::Real deltaTime){
	this->updateAnimations(deltaTime);	// Update animation playback
	this->updateLocomote(deltaTime);	// Update Locomotion

}

void Robot::updateLocomote(Ogre::Real deltaTime){
	mDirection = flockingNormal();//get the flocking velocity
	if (mDirection != Ogre::Vector3::ZERO){//if the velocity isnt zero set up animations
		if (robAnim != WALK){
			setAnimation(WALK);
		}
		mTimer = 0;
		//always rotating
		Ogre::Vector3 src = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_X;//rotate for first location
		if ((1.0f + src.dotProduct(mDirection)) < 0.0001f) 
		{
			mBodyNode->yaw(Ogre::Degree(180));
		}
		else
		{
			Ogre::Quaternion quat = src.getRotationTo(mDirection.normalisedCopy());
			mBodyNode->rotate(quat);
		}
		mBodyNode->translate(mDirection);
	}
	else{//when velocity is zero set idle animations
		if(robAnim != IDLE ){
			setAnimation(IDLE);
		}
		mTimer = 0;
	}
}

//Set movement flags based on a char to represent direction and boolean on or not
void Robot::setMovement(char dir, bool on){
	
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

Ogre::Vector3 Robot::flockingNormal(){				//need to add stuff to gameapplication
	Ogre::Vector3 seperation(0,0,0);
	Ogre::Vector3 alignment(0,0,0);
	Ogre::Vector3 cohesion(0,0,0);
	Ogre::Vector3 centerMass(0,0,0);
	Ogre::Vector3 vel;
	Ogre::Vector3 temp;
	Ogre::Vector3 temp2;
	Ogre::Vector3 diff;
	float diffMag;
	float dist;
	float radius = 100;
	float weight;
	float weightsum = 0;
	std::list<Robot*> agents = app->getRobotList();
	std::list<Robot*>::iterator aIter;
	Ogre::Vector3 omgwork = Ogre::Vector3::ZERO;
	//if the games walklist isn't empty
	omgwork = app->getHousePointer()->getPosition();
	//loop through agents
	temp = mBodyNode->getPosition();
	for (aIter = agents.begin(); aIter != agents.end(); aIter++){
		//to not pick itself
		if (this != (*aIter)){
			//calc the weight....maybe later
			weight = 1;
			temp2 = (*aIter)->getPosition();
			//get the distance between agents
			dist = sqrt(pow((temp[0] - temp2[0]), 2) + pow((temp[1] - temp2[1]), 2) + pow((temp[2] - temp2[2]), 2));
			//if distance is inside the radius of sight
			if (dist < radius){
				weight = 1 - (dist/radius); //weight should be higher the closer the object is
				//calculate seperation portion
				diff[0] = temp[0] - temp2[0];
				diff[1] = temp[1] - temp2[1];
				diff[2] = temp[2] - temp2[2];
				diffMag = sqrt(pow(diff[0], 2) + pow(diff[1],2) + pow(diff[2], 2));
				seperation += weight * (diff/(diffMag*diffMag));
				//calculate alignment portion	
				alignment += weight * (*aIter)->mDirection;
				weightsum += weight;
				//calculate center of mass for cohesion
				centerMass += weight * (*aIter)->getPosition();
			}
		}
	}
	//stupid weight sum fix
	if (weightsum != 0){
		//finishing alignment calc and setting height to zero so no flying
		alignment = alignment/weightsum;
		alignment[1] = 0;
		//finishing center of mass and cohesion and setting height to zero so no flying
		centerMass = centerMass/weightsum;
		cohesion = centerMass - mBodyNode->getPosition();
		cohesion[1] = 0;
	}
	//adding a force for moving towards a goal
	omgwork = omgwork - mBodyNode->getPosition();
	//if an agent is close enough to the goal pop it off the queue
	if(sqrt(pow(omgwork[0], 2) + pow(omgwork[1], 2) + pow(omgwork[2], 2)) < 40){
		return Ogre::Vector3::ZERO;
	}
	//normalise the difference vector so you can limit the speed better
	omgwork.normalise();
	omgwork[1] = 0;

	//the constants and parts of the velocity put together.
	vel = .01 * alignment + 2.5 * seperation + .1 * cohesion +  .05 * omgwork;

	return vel;
}

Ogre::Vector3 Robot::flockingFlee(){
	return Ogre::Vector3::ZERO;
}
Ogre::Vector3 Robot::flockingSeek(){
	return Ogre::Vector3::ZERO;
}
Ogre::Vector3 Robot::flockingLeader(){
	return Ogre::Vector3::ZERO;
}