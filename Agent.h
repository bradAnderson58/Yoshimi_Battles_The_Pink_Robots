#include "BaseApplication.h"
#include <deque>

#pragma once
class GameApplication;

class Agent
{
protected:
	Ogre::SceneManager* mSceneMgr;		// pointer to scene graph
	Ogre::SceneNode* mBodyNode;			
	Ogre::Entity* mBodyEntity;
	float height;						// height the character should be moved up
	float scale;						// scale of character from original model

	Ogre::Vector3 defaultPos;
	Ogre::Quaternion defaultOrient;		//reset the position and orientation to these values

	int successes;  //track the number of successful fish in barrels

	// lecture on physics
	bool projectile; // is this agent going to be launched?
	Ogre::Vector3 initPos; // initial position
	Ogre::Vector3 vel; // velocity of agent
	Ogre::Vector3 gravity; 
	
	//Ogre::ParticleSystem* ps;  //I like the particles, I will keep them

	GameApplication *app;
	
	// all of the animations our character has, and a null ID
	// some of these affect separate body parts and will be blended together
	//Sinbad animations
	virtual enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};

	Ogre::AnimationState* mAnims[21];		// master animation list
	AnimID mBaseAnimID;						// current base (full- or lower-body) animation
	AnimID mTopAnimID;						// current top (upper-body) animation
	bool mFadingIn[20];						// which animations are fading in
	bool mFadingOut[20];					// which animations are fading out
	Ogre::Real mTimer;						// general timer to see how long animations have been playing
	Ogre::Real mVerticalVelocity;			// for jumping

	virtual void setupAnimations();					// load this character's animations
	virtual void fadeAnimations(Ogre::Real deltaTime);				// blend from one animation to another
	virtual void updateAnimations(Ogre::Real deltaTime);			// update the animation frame

	// for locomotion
	Ogre::Real mDistance;					// The distance the agent has left to travel
	Ogre::Vector3 mDirection;				// The direction the object is moving
	Ogre::Vector3 mDestination;				// The destination the object is moving towards
	std::deque<Ogre::Vector3> mWalkList;	// The list of points we are walking to
	Ogre::Real mWalkSpeed;					// The speed at which the object is moving
	bool nextLocation();					// Is there another destination?
	virtual void updateLocomote(Ogre::Real deltaTime);			// update the character's walking

	//////////////////////////////////////////////
	// Lecture 4
	bool procedural;						// Is this character performing a procedural animation
    //////////////////////////////////////////////
public:
	Agent(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, GameApplication* a);
	~Agent();
	void setPosition(float x, float y, float z);

	virtual void update(Ogre::Real deltaTime);		// update the agent
	
	void setBaseAnimation(AnimID id, bool reset = false);	// choose animation to display
	void setTopAnimation(AnimID id, bool reset = false);

	void setApp(GameApplication *a) { app = a; }  //I need to access some stuff in the gameApplication from the agent
};