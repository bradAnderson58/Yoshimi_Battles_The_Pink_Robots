#ifndef __Yoshimi_h_
#define __Yoshimi_h_

#include "Agent.h"

class GameApplication;

class Yoshimi : public Agent{

public:

	Yoshimi(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, GameApplication* a);
	~Yoshimi();

	void update(Ogre::Real deltaTime);		// update hero!
	void updateLocomote(Ogre::Real deltaTime);
	void setMovement(char dir, bool on); //set the movemnt
	void rotationCode(OIS::MouseEvent arg);  //Rotate the hero when we move the mouse
	void buttonAnimation(char pressed);		//tell me what animation to do
	

	bool doingStuff;		//if Yoshimi is currently doing a non-looping animation
	void changeSpeed(double s) { speed = s; }	//change the speed for different animations

	void checkHits(char attack);  //for attacks - check if a robot gets hit
	void checkBump();
	Ogre::Vector3 getPosition(){return mBodyNode->getPosition();}
	void restart();
	void setInitPos(Ogre::Vector3 p){initPos = p;}

private:
	bool fForward; //how am I moving? Each flag indicates a direction
	bool fBackward;
	bool fRight;
	bool fLeft;
	double speed;	//how fast to move

	Ogre::SceneNode *mAttackNode;	//use this for bounding area for Yoshimi attacks
	Ogre::Entity *mAttackEntity;

	enum AnimID
	{
		ATTACK_ONE,
		ATTACK_TWO,
		ATTACK_THREE,
		BACKFLIP,
		BLOCK,
		CLIMB,
		CROUCH,
		DEATH_ONE,
		DEATH_TWO,
		HIGH_JUMP,
		IDLE_ONE,
		IDLE_TWO,
		IDLE_THREE,
		JUMP,
		JUMP_NO_HEIGHT,
		KICK,
		SIDE_KICK,
		SPIN,
		STEALTH,
		WALK,
		ANIM_NONE
	};

	AnimID yoshAnim;
	void setupAnimations();									// load this character's animations
	void fadeAnimations(Ogre::Real deltaTime);				// blend from one animation to another
	void updateAnimations(Ogre::Real deltaTime);			// update the animation frame
	void setAnimation(AnimID id, bool reset = false);
	void collisionRobots();
	void collisionWalls();

	

};

#endif