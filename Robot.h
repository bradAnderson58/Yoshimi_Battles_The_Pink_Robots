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
	void rotationCode(OIS::MouseEvent arg);  //Rotate the hero when we move the mouse

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

	AnimID robAnim;
	void setupAnimations();									// load this character's animations
	void fadeAnimations(Ogre::Real deltaTime);				// blend from one animation to another
	void updateAnimations(Ogre::Real deltaTime);			// update the animation frame
	void setAnimation(AnimID id, bool reset = false);

};

#endif