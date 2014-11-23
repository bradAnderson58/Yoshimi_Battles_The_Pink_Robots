#ifndef __GameApplication_h_
#define __GameApplication_h_

#include "BaseApplication.h"
#include "Agent.h"
#include "Yoshimi.h"
#include "Robot.h"
#include "Grid.h"

class GameApplication : public BaseApplication
{
private:
	Agent* agent; // store a pointer to the character
	std::list<Robot*> RobotList; // Lecture 5: now a list of agents

	Yoshimi* yoshPointer;	//This is our heroic savior, Yoshimi!
	Ogre::SceneNode* housePointer;
	std::list<Ogre::SceneNode*> wallList;
	Ogre::AxisAlignedBox boundBox;  //bounding box of the barrel

public:	

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

	Ogre::AxisAlignedBox getBox() { return boundBox; }  //return the bounding box of barrel

	Ogre::Camera* getCamera() { return mCamera; }  //why u no like?
	Yoshimi* getYoshimiPointer(){return yoshPointer;}
	std::list<Ogre::SceneNode*> getWallList(){return wallList;}
	std::list<Robot*> getRobotList(){return RobotList;}
	Ogre::SceneNode* getHousePointer(){return housePointer;}


protected:
    virtual void createScene(void);

	virtual void createGUI(void);  //for gui's

	void buttonHit(OgreBites::Button *b);

	void message();  //test this out real quick
	bool startGame;	//start button

	//GUI buttons
	OgreBites::Button *cont;
	OgreBites::Button *inst;
	OgreBites::Button *cred;
	OgreBites::TextBox *texty;
	OgreBites::Button *back;

	//Strings for GUI
	std::string instruction;
	std::string credits;
	
	//Path files for sounds
	std::string music;

	OgreBites::ParamsPanel* mParamsPanel;

};

#endif // #ifndef __TutorialApplication_h_
