#ifndef __GameApplication_h_
#define __GameApplication_h_

#include "BaseApplication.h"
#include "Agent.h"
#include "Yoshimi.h"
#include "Robot.h"
#include "Grid.h"

class Grid;  //okay whatever

class GameApplication : public BaseApplication
{
private:
	Agent* agent; // store a pointer to the character
	std::list<Robot*> RobotList; //now a list of robots

	Yoshimi* yoshPointer;	//This is our heroic savior, Yoshimi!
	Ogre::SceneNode* housePointer;		//point to the location of the house COM
	float houseHealth;					//when health gets to zero, much sad
	bool gameOver;

	std::list<Ogre::SceneNode*> wallList;
	std::list<Ogre::SceneNode*> borderWalls;
	Ogre::AxisAlignedBox boundBox;  //bounding box of the barrel

	//Boundaries of the world
	Grid *grid;
	float xMax, zMax;

public:	

    GameApplication(void);
    virtual ~GameApplication(void); 

	float getXmax() { return xMax; }
	float getZmax() { return zMax; }

	void loadEnv();			// Load the buildings or ground plane, etc.
	void setupEnv();		// Set up the lights, shadows, etc
	void loadObjects();		// Load other props or objects (e.g. furniture)
	void loadCharacters();	// Load actors, agents, characters

	Grid* getGrid(){ return grid; } //get it

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

	void endGame(char condition);		//End the game in either victory or crushing defeat
	void destroyallChildren(Ogre::SceneNode* p);

protected:
    virtual void createScene(void);

	virtual void createGUI(void);  //for gui's

	void buttonHit(OgreBites::Button *b);

	bool startGame;	//start button

	//GUI buttons and such
	OgreBites::Button *cont;
	OgreBites::Button *inst;
	OgreBites::Button *cred;
	OgreBites::TextBox *texty;
	OgreBites::Button *back;
	OgreBites::ProgressBar *houseHUD;

	//Strings for GUI
	std::string instruction;
	std::string credits;
	
	//Path files for sounds
	std::string music;

	OgreBites::ParamsPanel* mParamsPanel;

};

#endif // #ifndef __TutorialApplication_h_
