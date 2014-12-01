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
	//mSceneMgr->showBoundingBoxes(true);                        //for robots
	//mBodyNode->showBoundingBox(false);
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
	state = NORMAL;
	flying = false;
	dead = false;
	atLocation = false;
	fleeSet = false;
	closeFriendDied = false;
	playingShoot = false;
	health = 100;
	if(rand() % 2 == 0){
		goRight = true;
	}
	else{
		goRight = false;
	}

	Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout

	//Rainbow Guns
	ps = mSceneMgr->createParticleSystem(mBodyNode->getName() + "fountain", "Examples/PurpleFountain");
	Ogre::SceneNode* mnode = mBodyNode->createChildSceneNode();
	mnode->roll(Ogre::Degree(-90));
	mnode->setPosition(0,50,0);
	mnode->attachObject(ps);
	ps->setVisible(false);

	//File sound dissappinting :(
	shoot = __FILE__; //gets the current cpp file's path with the cpp file
	shoot = shoot.substr(0,1+shoot.find_last_of('\\')); //removes filename to leave path
	shoot += "\\Sounds\\robotShooting.wav";

	die = __FILE__; //gets the current cpp file's path with the cpp file
	die = shoot.substr(0,1+shoot.find_last_of('\\')); //removes filename to leave path
	die += "\\Sounds\\r2d2dying.wav";
}

Robot::~Robot(void)
{
	
}

void Robot::update(Ogre::Real deltaTime){
	if (health < 30){
		state = HURT;
	}
	this->updateAnimations(deltaTime);	// Update animation playback
	if (!flying && !dead){
										  	// Update animation playback
		this->updateLocomote(deltaTime);	// Update Locomotion
		this->RobotCollisions();
		if (atLocation){
			if (robAnim != SHOOT){
				this->setAnimation(SHOOT);
				ps->setVisible(true);
			}
			/*if (!playingShoot){
				PlaySound(shoot.c_str(), NULL, SND_FILENAME|SND_ASYNC|SND_LOOP);
				playingShoot = true;
			}*/
		}
		/*else{
			PlaySound(NULL, 0,SND_ASYNC);
			playingShoot = false;
		}*/
	}

	//Knockback code (similar to fish 'shoot' method)
	else if (!dead){
		checkBoundaryCollision();
		using namespace Ogre;
		Vector3 pos = mBodyNode->getPosition();
		mDirection = mDirection + (gravity * deltaTime);
		pos = pos + (mDirection * deltaTime); // velocity
		pos = pos + 0.5 * gravity * deltaTime * deltaTime; // acceleration

		this->mBodyNode->setPosition(pos);

		if (this->mBodyNode->getPosition().y <= 0){
			mBodyNode->setPosition(getPosition().x, 0, getPosition().z);
			//Sometimes this gets messed up here
			mDirection.y = 0;
			flying = false;
			if (health <= 0) setDeath();
		}
	}
}

void Robot::updateLocomote(Ogre::Real deltaTime){
	Ogre::Vector3 yoshPos = app->getYoshimiPointer()->getPosition();
	if (state == NORMAL){
		mDirection = flockingNormal();
	}
	else if (state == HURT){
		Ogre::Real dist = mBodyNode->getPosition().distance(yoshPos);
		if (dist < 20){
			fleeSet = true;
			mDirection = flockingFlee();
		}
		else if (dist > 30){
			fleeSet = false;
			mDirection = flockingNormal();
		}
	}
	else if (state == ANGRY){
		Ogre::Real dist = mBodyNode->getPosition().distance(yoshPos);
		if (dist < 50){
			mDirection = flockingSeek();
			fleeSet = true;
		}
		else{
			mDirection = flockingNormal();
			fleeSet = false;
		}
	}
	else{
		mDirection = Ogre::Vector3::ZERO;
	}
	if (mDirection != Ogre::Vector3::ZERO){//if the velocity isnt zero set up animations
		if (robAnim != WALK){
			setAnimation(WALK);
		}
		mTimer = 0;

		//check wall issues
		checkBoundaryCollision();     

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
		//mDirection = mDirection * 100;
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
		if (mAnims[robAnim]->hasEnded()){
			setAnimation(ANIM_NONE);			//robot slumps when hit, but then reset to none
		}
	}
	
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
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

//Going around the house - don't go where there are already robots
Ogre::Vector3 Robot::getSpecificPos(){
	Ogre::Vector3 housePos = app->getHousePointer()->getPosition();
	std::list<Robot*> robots = app->getRobotList();

	std::list<Ogre::Vector3> occupied;
	std::list<Ogre::Vector3>::iterator oIter;

	std::list<Robot*>::iterator rIter;

	//Robots will go to a specific position based on a radius around the house
	double angle = 0;
	Ogre::Real c = cos(angle);
	Ogre::Real s = sin(angle);
	Ogre::Matrix3 rot(c,0,s,0,1,0,-s,0,c);

	//Check if a position is occupied
	for (rIter = robots.begin(); rIter != robots.end(); rIter++){
		if ( !(*rIter)->notAtLocation()){
			occupied.push_back((*rIter)->getPosition());
		}
	}

	bool tooClose = false;
	bool endLoop = true;
	Ogre::Vector3 ret(0,0,0);
	Ogre::Vector3 temp(0,0,0);
	Ogre::Real dist = 0;

	//random direction
	bool addAngle = false;
	if(rand() % 2 == 0){
		addAngle = true;
	}

	temp = mBodyNode->getPosition() - housePos;
	temp.normalise();
	ret = housePos + (temp * 40);
	ret[1] = 0;

	do{
		//for each position in the occupied list - check if its too close
		for (Ogre::Vector3 thing : occupied){
			temp = thing - ret;
			dist = temp.length();
			if (dist < 2.5){
				tooClose = true;
			}
		}

		//move the position around the circumference
		if (tooClose){
			if(goRight){
				angle = -.1;
			}
			else{
				angle += .1;
			}
			c = cos(angle);
			s = sin(angle);
			rot = Ogre::Matrix3(c,0,s,0,1,0,-s,0,c);
			temp = ret - housePos;
			temp = temp * rot;
			ret = temp + housePos;
			tooClose = false;
		}
		else{
			endLoop = false;
		}
	}while (endLoop);	
	return ret;

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
	float radius = 20;
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
		//Also: dont use robots who are in the air, it screws everything up!
		if (this != (*aIter) && (*aIter)->notFlying() && (*aIter)->notDead() && (*aIter)->notAtLocation() && (*aIter)->notFleeing()){
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
				diffMag = diff.length();
				if (diffMag != 0){
					seperation += weight * (diff/(diffMag*diffMag));
				}
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
	Ogre::Real distance = omgwork.normalise();
	//if an agent is close enough to the goal pop it off the queue
	if(distance < 40){
		atLocation = true;
		return Ogre::Vector3::ZERO;
	}
	else if (distance < 100){
		atLocation = false;
		
		omgwork = getSpecificPos();
		omgwork = omgwork - mBodyNode->getPosition();
		omgwork.normalise();
		if (distance < 60){
			return omgwork * .05;
		}
	}
	else{
		atLocation = false;
	}
	//normalise the difference vector so you can limit the speed better
	omgwork[1] = 0;

	//the constants and parts of the velocity put together.
	vel = .01 * alignment + 1.5 * seperation + .01 * cohesion +  .05 * omgwork;

	return vel;
}

//Algorithm for fleeing robots
Ogre::Vector3 Robot::flockingFlee(){

	//run in the opposite direction of Yoshimi
	Ogre::Vector3 yoshPos = app->getYoshimiPointer()->getPosition();
	Ogre::Vector3 mPosition = mBodyNode->getPosition();
	Ogre::Vector3 desired = (mPosition - yoshPos);
	desired.normalise();
	desired *= .05;
	desired[1] = 0;
	/*Ogre::Vector3 steer = desired - mDirection;
	steer[1] = 0;
	steer += mDirection*/							//if you wanna make it impossible to catch the guy;
	return desired;
}

//Seek is opposite of flee
Ogre::Vector3 Robot::flockingSeek(){
	return flockingFlee() * -1;
}

Ogre::Vector3 Robot::flockingLeader(){
	return Ogre::Vector3::ZERO; //hasnt been implemented - no leader
}

//Here the robot reacts when hit by Yoshimi
void Robot::getHit(char attack, Ogre::Vector3 dir){

	//different reactions for different attacks
	//This is sword attack
	if (attack == 's'){
		setAnimation(SLUMP, true);
		health -= 10;
		setFlyback(5, dir);

	}else if (attack == 'k'){
		setAnimation(SLUMP, true);
		health -= 20;
		setFlyback(15, dir);
	}
	ps->setVisible(false);	//no longer shooting while hit
}

//Similar to 'fire' method in fish game.  We will knock this robot back
void Robot::setFlyback(int velocity,Ogre::Vector3 dir){
	flying = true;
	atLocation = false;
	// set up the initial state
	flyPos = mBodyNode->getPosition();
	mDirection.x = -dir[0] * velocity;									
	mDirection.y = 0.707 * velocity;				//y and z values are determined by the angle of trajectory multiplied by velocity
	mDirection.z = -dir[2] * velocity;

	//gravity
	gravity.x = 0;
	gravity.y = -9.81;
	gravity.z = 0;
}

//do things when the robot dies
void Robot::setDeath(){

	//Tell other robots im dead - they will get angry
	std::list<Robot*> robots = app->getRobotList();
	for (Robot* guy : robots){
		if (getPosition().distance(guy->getPosition()) < 20){
			guy->setAngry();
		}
	}

	//flags
	atLocation = false;
	setAnimation(DIE);
	dead = true;

}

//check if I'm going outside of the boundaries
void Robot::checkBoundaryCollision(){
	
	float xBound = (app->getXmax() * 10) - 5;
	float zBound = (app->getZmax() * 10) - 5;
	Ogre::Vector3 myPos = mBodyNode->getPosition();
	Ogre::Vector3 house = app->getHousePointer()->getPosition();
	house.y = 0;  //to prevent crazies

	if (myPos.x <= -xBound){
		//hit bounds in x direction
		mBodyNode->setPosition(-xBound, myPos.y, myPos.z);
		mDirection.x = -mDirection.x;
	}else if ( myPos.x >= xBound){

		mBodyNode->setPosition(xBound, myPos.y, myPos.z);
		mDirection.x = -mDirection.x;
	}
	if (myPos.z <= -zBound){
		//hit bounds in z direction
		mBodyNode->setPosition( myPos.x, myPos.y, -zBound);
		mDirection.z = -mDirection.z;
		
	}else if (myPos.z >= zBound){
		
		mBodyNode->setPosition( myPos.x, myPos.y, zBound);
		mDirection.z = -mDirection.z;
	}

	//Do house checking as well
	//First check if in location, then do more maths
	float dist = myPos.distance(house);
	float xDist = myPos.x - house.x;
	float zDist = myPos.z - house.z;

	if (dist < 25){

		if ((xDist <= 15.0 && xDist >= -18.0) && (zDist <= 18.0 && zDist >= -18.0)){ //robot hits the house

			//We hit the house in the x direction
			if (abs(xDist) > abs(zDist)){
				
				//Right side of the house
				if (xDist > 0) mBodyNode->setPosition(house.x + 15, myPos.y, myPos.z);
				
				//Left side of the house
				else mBodyNode->setPosition(house.x - 18, myPos.y, myPos.z);

				mDirection.x = -mDirection.x;
			//We hit the house in the z direction
			}else{

				//Front of  the house?
				if ( zDist > 0) mBodyNode->setPosition(myPos.x, myPos.y, house.z + 18);
				
				//Back of the house
				else mBodyNode->setPosition(myPos.x, myPos.y, house.z - 18);
				
				mDirection.z = -mDirection.z;
			}
		}
	}
}

//Checkcollisions between robots
void Robot::RobotCollisions(){
	Ogre::Vector3 temp(0,0,0);
	Ogre::Vector3 pos = mBodyNode->getPosition();
	Ogre::Vector3 rPos(0,0,0);
	std::list<Robot*> robots = app->getRobotList();

	//For each other robot - check if robots are too close to each other
	for (Robot* rob : robots){
		if (rob != this && rob->notDead()){
			rPos = rob->getPosition();

			//collision - move me over
			if (mBodyNode->getPosition().distance(rob->getPosition()) < 3){
				temp = pos - rPos;
				temp.normalise();
				temp = rPos + (temp * 3);
				temp[1] = 0;
				setPosition(temp[0], 0, temp[2]);
			}
		}
	}
}

//Reset the robot for restarting the level
void Robot::restart(){
	mBodyNode->setPosition(initPos);
	state = NORMAL;
	flying = false;
	dead = false;
	atLocation = false;
	fleeSet = false;
	closeFriendDied = false;
	playingShoot = false;
	health = 100;
	setAnimation(DIE, true);  //reset die animation fixes reload bug
	ps->setVisible(false);	//make sure these fellas stop shooting
}