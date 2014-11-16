#ifndef __GameApplication_h_
#define __GameApplication_h_

#include "BaseApplication.h"
#include "Agent.h"
#include "Yoshimi.h"


class GameApplication : public BaseApplication
{
private:
	Agent* agent; // store a pointer to the character
	std::list<Agent*> agentList; // Lecture 5: now a list of agents

	Yoshimi* yoshPointer;	//This is our heroic savior, Yoshimi!
	
	//these values will be passed to the fire method to fire our fish
	int trajVal;
	int velVal;

	OgreBites::Slider *mSlider; // I want to mess with this in other places as well
	bool pos;  //slider moves back and forth
	int slowMe;  //move slider bar
	int lightMe; //change lighting

	Ogre::AxisAlignedBox boundBox;  //bounding box of the barrel

public:
	enum stateState				//we gonna press the space bar for everything so this will be that
	{
		TRAJECTORY,
		VELOCITY,
		FIRE,
		ANIMATE
	};
	enum colorVal			//Change lighting to indicate hits and misses
	{
		RED,
		GREEN,
		WHITE
	};

	void changeLighting(colorVal change);	

    GameApplication(void);
    virtual ~GameApplication(void);

	void loadEnv();			// Load the buildings or ground plane, etc.
	void setupEnv();		// Set up the lights, shadows, etc
	void loadObjects();		// Load other props or objects (e.g. furniture)
	void loadCharacters();	// Load actors, agents, characters

	void addTime(Ogre::Real deltaTime);		// update the game state

	//////////////////////////////////////////////////////////////////////////
	// Lecture 4: keyboard interaction
	// moved from base application
	// OIS::KeyListener
    bool keyPressed( const OIS::KeyEvent &arg );
    bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    bool mouseMoved( const OIS::MouseEvent &arg );
    bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////
	// Lecture 12
	bool bLMouseDown, bRMouseDown;		//true if mouse buttons are held down
	Ogre::SceneNode *mCurrentObject;	//pointer to our currently selected object

	/////////////////////////////////////////////////////////////////////////////////

	void setState(stateState st) { state = st; };	//set the state from outside class

	void slideCaption(std::string labl) { mSlider->setCaption(labl); }  //set the slider label from outside class

	void setSuccess(int num);	//set update the number of success

	Ogre::AxisAlignedBox getBox() { return boundBox; }  //return the bounding box of barrel

	Ogre::Camera* getCamera() { return mCamera; }  //why u no like?

protected:
    virtual void createScene(void);

	virtual void createGUI(void);  //for gui's
	OgreBites::ParamsPanel* mParamsPanel;

private:
	stateState state;		//lol
	colorVal lightColor;	//keep track of what color the lighting is
	Ogre::Light *mLight;	//keep this light around too
};

#endif // #ifndef __TutorialApplication_h_
