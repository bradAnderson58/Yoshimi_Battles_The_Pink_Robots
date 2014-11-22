#ifndef __Robot_h_
#define __Robot_h_

#include "Agent.h"

class GameApplication;

class Robot : public Agent{

public:

	int counter;
	int testing;
	

	Robot(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, GameApplication* a);
	~Robot();

	void update(Ogre::Real deltaTime);		// update hero!
	void updateLocomote(Ogre::Real deltaTime);
	void setMovement(char dir, bool on); //set the movemnt
	Ogre::Vector3 getPosition(){return mBodyNode->getPosition();}

	Ogre::AxisAlignedBox getBoundingBox() { return mBodyEntity->getWorldBoundingBox(); }   //Yoshimi checks this to see if hit
	void getHit(char attack, Ogre::Vector3 dir);		//Here will be code to react to being hit by Yoshimi
	void setFlyback(int velocity,  Ogre::Vector3 dir);		//Here we will knock a robot back
	void setDeath();

	bool notFlying() { return !flying; }  //returns true if flying is false -dp exclusive
	bool notDead() { return !dead; }     //or dead

private:

	enum AnimID
	{
		DIE,
		IDLE,
		SHOOT,
		SLUMP,
		WALK,
		ANIM_NONE
	};

	enum State{
		NORMAL,
		HURT,
		ANGRY,
		BOSS,
		FOLLOWER
	};

	State state;
	AnimID robAnim;
	Ogre::Vector3 mDirection;
	void setupAnimations();									// load this character's animations
	void fadeAnimations(Ogre::Real deltaTime);				// blend from one animation to another
	void updateAnimations(Ogre::Real deltaTime);			// update the animation frame
	void setAnimation(AnimID id, bool reset = false);

	Ogre::Vector3 flockingNormal();
	Ogre::Vector3 flockingFlee();
	Ogre::Vector3 flockingSeek();
	Ogre::Vector3 flockingLeader();

	bool flying;		//if the robot is in the process of being knocked back
	int health;			//this is robot health
	bool dead;			//dead robot
};

#endif