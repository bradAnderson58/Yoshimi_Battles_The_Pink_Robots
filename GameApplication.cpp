#include "GameApplication.h"
#include <fstream>
#include <sstream>
#include <map> 

//-------------------------------------------------------------------------------------
GameApplication::GameApplication(void): 
	mCurrentObject(0),
	bLMouseDown(false),
	bRMouseDown(false)
{
	agent = NULL; // Init member data
	housePointer = NULL;
	startGame = false;
	houseHealth = 1.0f;
	gameOver = false;
	level = 0;
	
	//set up these strings for later usages
	std::stringstream ins;	// a stream for outputing to a string (why no clear()?)
	std::stringstream cre;
	ins << "Your name is Yoshimi, you're a blackbelt in Karate";
	ins << "\nMOVE: \nWASD moves Yoshimi as you would expect.  \nMouse movements rotate Yoshimi to face different directions.";
	ins <<"\nATTACK: \nLeft mouse click executes Judo Sword Attack, right mouse click executes Epic Front Kick.";
	ins <<"\nOBJECTIVE:  \nYou must protect your house from the evil natured Robots, who are programmed to destroy us!";
	instruction = ins.str();
	//out.clear();

	cre << "Inspiration and music taken from \"Yoshimi Battles the Pink Robots\" by The Flaming Lips.";
	cre << "\nGraphics made with assistance of the Ogre Rendering Engine API.";
	cre << "\nLEAD DEVELOPERS:  \nBrad Anderson and Kevin Dec, \nDangling Pointers LLC";
	credits = cre.str();

	music = __FILE__; //gets the current cpp file's path with the cpp file
	music = music.substr(0,1+music.find_last_of('\\')); //removes filename to leave path
	music+= "\\Sounds\\YBPR_part2.wav"; //if txt file is in the same directory as cpp file

}
//-------------------------------------------------------------------------------------
GameApplication::~GameApplication(void)
{
	if (agent != NULL)  // clean up memory
		delete agent; 
}

//-------------------------------------------------------------------------------------
void GameApplication::createScene(void)
{
    /*loadEnv();
	setupEnv();*/  //DO THIS ELSEWHERE?
	//loadObjects();
	//loadCharacters();
	//////////////////////////////////////////////////////////////////////////////////
	// Lecture 12
	//but we also want to set up our raySceneQuery after everything has been initialized
	mRayScnQuery = mSceneMgr->createRayQuery(Ogre::Ray());

}
//////////////////////////////////////////////////////////////////
// Returns a unique name for loaded objects and agents
std::string getNewName() // return a unique name 
{
	static int count = 0;	// keep counting the number of objects

	std::string s;
	std::stringstream out;	// a stream for outputing to a string
	out << count++;			// make the current count into a string
	s = out.str();

	return "object_" + s;	// append the current count onto the string
}

// load level from file
void // Load the buildings or ground plane, etc
GameApplication::loadEnv()
{
	using namespace Ogre;	// use both namespaces
	using namespace std;

	class readEntity // need a structure for holding entities
	{
	public:
		string filename;
		float y;
		float scale;
		float orient;
		bool agent;
	};

	PlaySound(music.c_str(), NULL, SND_FILENAME|SND_ASYNC);  //Game sound
	//mInputManager->destroyInputObject(mMouse);
	//mMouse = NULL;								//How to hide the mouse?
	//mMouse->getMouseState();
	
	GridNode *temp;

	ifstream inputfile;		// Holds a pointer into the file

	string path = __FILE__; //gets the current cpp file's path with the cpp file
	path = path.substr(0,1+path.find_last_of('\\')); //removes filename to leave path
	path+= "level001.txt"; //if txt file is in the same directory as cpp file
	inputfile.open(path);

	//inputfile.open("D:/CS425-2012/Lecture 8/GameEngine-loadLevel/level001.txt"); // bad explicit path!!!
	if (!inputfile.is_open()) // oops. there was a problem opening the file
	{
		cout << "ERROR, FILE COULD NOT BE OPENED" << std::endl;	// Hmm. No output?
		return;
	}

	// the file is open
	int x,z;
	inputfile >> x >> z;	// read in the dimensions of the grid
	string matName;
	inputfile >> matName;	// read in the material name

	// create floor mesh using the dimension read
	MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Plane(Vector3::UNIT_Y, 0), x*NODESIZE, z*NODESIZE, x, z, true, 1, x, z, Vector3::UNIT_Z);
	
	//create a floor entity, give it material, and place it at the origin
	Entity* floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName(matName);
	floor->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(floor);

	grid = new Grid(mSceneMgr, z, x); // Set up the grid. z is rows, x is columns
	
	string buf;
	inputfile >> buf;	// Start looking for the Objects section
	while  (buf != "Objects")
		inputfile >> buf;
	if (buf != "Objects")	// Oops, the file must not be formated correctly
	{
		cout << "ERROR: Level file error" << endl;
		return;
	}

	// read in the objects
	readEntity *rent = new readEntity();	// hold info for one object
	std::map<string,readEntity*> objs;		// hold all object and agent types;
	while (!inputfile.eof() && buf != "Characters") // read through until you find the Characters section
	{ 
		inputfile >> buf;			// read in the char
		if (buf != "Characters")
		{
			inputfile >> rent->filename >> rent->y >> rent->orient >> rent->scale;  // read the rest of the line
			rent->agent = false;		// these are objects
			objs[buf] = rent;			// store this object in the map
			rent = new readEntity();	// create a new instance to store the next object
		}
	}

	while  (buf != "Characters")	// get through any junk
		inputfile >> buf;
	
	// Read in the characters
	while (!inputfile.eof() && buf != "World") // Read through until the world section
	{
		inputfile >> buf;		// read in the char
		if (buf != "World")
		{
			inputfile >> rent->filename >> rent->y >> rent->scale; // read the rest of the line
			rent->agent = true;			// this is an agent
			objs[buf] = rent;			// store the agent in the map
			rent = new readEntity();	// create a new instance to store the next object
		}
	}
	delete rent; // we didn't need the last one

	// read through the placement map
	char c;
	for (int i = 0; i < z; i++)			// down (row)
		for (int j = 0; j < x; j++)		// across (column)
		{
			inputfile >> c;			// read one char at a time
			buf = c + '\0';			// convert char to string
			rent = objs[buf];		// find cooresponding object or agent
			if (rent != NULL){		// it might not be an agent or object
				if (rent->agent)	// if it is an agent...
				{
					// Use subclasses instead!
					if (c == 'n') {
						agent = new Yoshimi(this->mSceneMgr, getNewName(), rent->filename, rent->y, rent->scale, this);
						yoshPointer = (Yoshimi*) agent;  //you are a yoshimi
						agent->setPosition(grid->getPosition(i,j).x, 0, grid->getPosition(i,j).z);
						yoshPointer->setInitPos(yoshPointer->getPosition());
					}else {
						Robot* robot = new Robot(this->mSceneMgr, getNewName(), rent->filename, rent->y, rent->scale, this);
						robot->setPosition(grid->getPosition(i,j).x, 0, grid->getPosition(i,j).z);
						robot->setInitPos(robot->getPosition());
						RobotList.push_back(robot);
					}
					//agent->setApp(this);  //in constructor
					
				}
				else	// Load objects
				{
					if (rent->filename == "tudorhouse.mesh"){
						String work = getNewName();
						//temp = grid->loadObject(work, rent->filename, i, rent->y, j, rent->scale);
						//housePointer = mSceneMgr->getSceneNode(work);

						Entity *ent = mSceneMgr->createEntity(work, rent->filename);
						
						housePointer = mSceneMgr->getRootSceneNode()->createChildSceneNode(work,
							grid->getPosition(i,j));
						housePointer->attachObject(ent);
						housePointer->setScale(rent->scale, rent->scale, rent->scale);
						housePointer->translate(0,rent->y,0);
					}
					else {//The temp object holds a pointer to the barrel node, which we need for bounding box access
						temp = grid->loadObject(getNewName(), rent->filename, i, rent->y, j, rent->scale);
					}
				}
					
			}
			else // not an object or agent
			{
				if (c == 'w') // create a wall
				{
					Entity* ent = mSceneMgr->createEntity(getNewName(), Ogre::SceneManager::PT_CUBE);
					ent->setMaterialName("Examples/RustySteel");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ent);
					mNode->scale(0.1f,0.2f,0.1f); // cube is 100 x 100
					grid->getNode(i,j)->setOccupied();  // indicate that agents can't pass through
					mNode->setPosition(grid->getPosition(i,j).x, 10.0f, grid->getPosition(i,j).z);
					wallList.push_back(mNode);
				}
				else if (c == 'i') // create a invisible wall
				{
					Entity* ent = mSceneMgr->createEntity(getNewName(), Ogre::SceneManager::PT_CUBE);
					ent->setMaterialName("Examples/RustySteel");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ent);
					mNode->scale(0.1f,0.2f,0.1f); // cube is 100 x 100
					grid->getNode(i,j)->setOccupied();  // indicate that agents can't pass through
					mNode->setPosition(grid->getPosition(i,j).x, 10.0f, grid->getPosition(i,j).z);
					mNode->setVisible(false);
					wallList.push_back(mNode);
				}
				else if (c == 'b') // create borderwalls
				{
					Entity* ent = mSceneMgr->createEntity(getNewName(), Ogre::SceneManager::PT_CUBE);
					ent->setMaterialName("Examples/RustySteel");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ent);
					mNode->scale(0.1f,0.2f,0.1f); // cube is 100 x 100
					grid->getNode(i,j)->setOccupied();  // indicate that agents can't pass through
					mNode->setPosition(grid->getPosition(i,j).x, 10.0f, grid->getPosition(i,j).z);
					mNode->setVisible(true);
					borderWalls.push_back(mNode);
				}
				else if (c == 'e')
				{
					ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
					ParticleSystem* ps = mSceneMgr->createParticleSystem(getNewName(), "Examples/PurpleFountain");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ps);
					mNode->setPosition(grid->getPosition(i,j).x, 0.0f, grid->getPosition(i,j).z);
				}
			}
		}
	
	// delete all of the readEntities in the objs map
	rent = objs["s"]; // just so we can see what is going on in memory (delete this later)
	
	std::map<string,readEntity*>::iterator it;
	for (it = objs.begin(); it != objs.end(); it++) // iterate through the map
	{
		delete (*it).second; // delete each readEntity
	}
	objs.clear(); // calls their destructors if there are any. (not good enough)
	
	inputfile.close();
	//grid->printToFile(); // see what the initial grid looks like.

	//set up world boundaries
	xMax = (grid->getRows() - 1) / 2;
	zMax = (grid->getCols() - 1) / 2;

	agent = NULL;	//this gets deleted by yoshPointer or RobotList
}

void // Set up lights, shadows, etc
GameApplication::setupEnv()
{
	using namespace Ogre;

	// set shadow properties
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowTextureSize(1024);
	mSceneMgr->setShadowTextureCount(1);

	// disable default camera control so the character can do its own 
	mCameraMan->setStyle(OgreBites::CS_FREELOOK); // CS_FREELOOK, CS_ORBIT, CS_MANUAL

	// use small amount of ambient lighting
	mSceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));

	// add a bright light above the scene
	Ogre::Light* mLight = mSceneMgr->createLight();
	mLight->setType(Light::LT_POINT);
	mLight->setPosition(-10, 40, 20);
	mLight->setSpecularColour(ColourValue::White);
	mLight->setDiffuseColour(ColourValue::White);

	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8); // pretty sky
}

void
GameApplication::addTime(Ogre::Real deltaTime)
{
	if (!gameOver){
		int dead = 0;  //see how many robots are dead
		std::string comment = "Safe";
		// Iterate over the list of agents (robots)
		std::list<Robot*>::iterator iter;
		for (iter = RobotList.begin(); iter != RobotList.end(); iter++){
			if (*iter != NULL){
				(*iter)->update(deltaTime);
				if (!(*iter)->notAtLocation() && !gameOver){
					houseHealth -= .001;
					houseHUD->setProgress(houseHUD->getProgress() - .001);
					comment = "Under Attack!";
				}
			}
			if (!(*iter)->notDead()) dead++;
		}
		if (!gameOver) houseHUD->setComment(comment);  //warn if house is under attack

		if (startGame) yoshPointer->update(deltaTime); //Yoshimi has a different update function
	
		if (houseHealth <= 0 && !gameOver) endGame('l');
		if (dead != 0 && dead == RobotList.size() && !gameOver) endGame('w'); 
	}
}

bool 
GameApplication::keyPressed( const OIS::KeyEvent &arg ) // Moved from BaseApplication
{
    if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
    {
        mTrayMgr->toggleAdvancedFrameStats();
    }
    else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
    {
        if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
            mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
            mDetailsPanel->show();
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mDetailsPanel);
            mDetailsPanel->hide();
        }
    }
    else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
        {
        case 'B':
            newVal = "Trilinear";
            tfo = Ogre::TFO_TRILINEAR;
            aniso = 1;
            break;
        case 'T':
            newVal = "Anisotropic";
            tfo = Ogre::TFO_ANISOTROPIC;
            aniso = 8;
            break;
        case 'A':
            newVal = "None";
            tfo = Ogre::TFO_NONE;
            aniso = 1;
            break;
        default:
            newVal = "Bilinear";
            tfo = Ogre::TFO_BILINEAR;
            aniso = 1;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
        mDetailsPanel->setParamValue(9, newVal);
    }
    else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch (mCamera->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            newVal = "Wireframe";
            pm = Ogre::PM_WIREFRAME;
            break;
        case Ogre::PM_WIREFRAME:
            newVal = "Points";
            pm = Ogre::PM_POINTS;
            break;
        default:
            newVal = "Solid";
            pm = Ogre::PM_SOLID;
        }

        mCamera->setPolygonMode(pm);
        mDetailsPanel->setParamValue(10, newVal);
    }
    else if(arg.key == OIS::KC_F5)   // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
	else if (arg.key == OIS::KC_SPACE)
	{
		if(!yoshPointer->doingStuff){
			yoshPointer->changeSpeed(.6);  //jump should be slower
			yoshPointer->buttonAnimation('j');
			yoshPointer->doingStuff = true;
		}
	}
	else if (arg.key == OIS::KC_W) {
		
		if (startGame) yoshPointer->setMovement('f', true);
	}
	else if (arg.key == OIS::KC_A) {
		if (startGame) yoshPointer->setMovement('l', true);
	}
	else if (arg.key == OIS::KC_D) {
		if (startGame) yoshPointer->setMovement('r', true);
	
	}
	else if (arg.key == OIS::KC_S) {
		if (startGame) yoshPointer->setMovement('b', true);
	}
	//Some wicked attacks
	else if (arg.key == OIS::KC_Q){
		if(!yoshPointer->doingStuff){
			yoshPointer->buttonAnimation('t');
			yoshPointer->doingStuff = true;
		}
	}
   
    //mCameraMan->injectKeyDown(arg);
    return true;
}

bool GameApplication::keyReleased( const OIS::KeyEvent &arg )
{
	//Set the flag to false for whichever key is no longer pressed
	if (startGame){
		if (arg.key == OIS::KC_W) yoshPointer->setMovement('f', false);
		else if (arg.key == OIS::KC_A) yoshPointer->setMovement('l', false);
		else if (arg.key == OIS::KC_S) yoshPointer->setMovement('b', false);
		else if (arg.key == OIS::KC_D) yoshPointer->setMovement('r', false);
	}
    //mCameraMan->injectKeyUp(arg);
    return true;
}

bool GameApplication::mouseMoved( const OIS::MouseEvent &arg )
{

	if (startGame) yoshPointer->rotationCode(arg);
	
    if (mTrayMgr->injectMouseMove(arg)) return true;
    //mCameraMan->injectMouseMove(arg);
    return true;
}

bool GameApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	/////////////////////////////////////////////////////////////////////////////////////
	// attack using left and right mouse buttons once the game starts
	if (startGame){
		if(id == OIS::MB_Left && !yoshPointer->doingStuff)
		{
			yoshPointer->changeSpeed(1);
			yoshPointer->buttonAnimation('s');
			yoshPointer->doingStuff = true;
			yoshPointer->checkHits('s');
		}
		else if (id == OIS::MB_Right && !yoshPointer->doingStuff){
			yoshPointer->buttonAnimation('k');
			yoshPointer->doingStuff = true;
			yoshPointer->checkHits('k');
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
   
	if (id == OIS::MB_Right) bRMouseDown = false;
	else if (id == OIS::MB_Left) bLMouseDown = false;
    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    //mCameraMan->injectMouseDown(arg, id);
    return true;
}

bool GameApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (id == OIS::MB_Right) bRMouseDown = false;
	else if (id == OIS::MB_Left) bLMouseDown = false;
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
    return true;
}

void GameApplication::createGUI(void)
{
	//////////////////////////////////////////////////////////////////////////////////
	
	if (mTrayMgr == NULL) return;
	using namespace OgreBites; 
	
	//Set up our GUI buttons------------------------------------------------

	OgreBites::Label *title = mTrayMgr->createLabel(TL_CENTER, "Title", "Yoshimi Battles The Pink Robots", 500.0f);
	mTrayMgr->createSeparator(TL_CENTER,"sep", 500.0f);
	
	cont = mTrayMgr->createButton(TL_CENTER, "ClickMe", "Play", 200.0);
	mTrayMgr->buttonHit(cont);

	inst = mTrayMgr->createButton(TL_CENTER, "instruct", "Instructions", 200.0);
	mTrayMgr->buttonHit(inst);

	cred = mTrayMgr->createButton(TL_CENTER, "credit", "Credits", 200.0);
	mTrayMgr->buttonHit(cred);

	texty = mTrayMgr->createTextBox(TL_CENTER, "text", "", 350.0, 200.0);
	texty->hide();

	back = mTrayMgr->createButton(TL_CENTER, "back", "Back", 200.0);
	mTrayMgr->buttonHit(back);
	back->hide();

	//House HUD will not display until game starts - decrement health when robots attack
	houseHUD = mTrayMgr->createProgressBar(TL_TOP, "househealth", "HOUSE", 350.0f, 200.0f);
	houseHUD->setComment("Safe");
	houseHUD->setProgress(1);
	houseHUD->hide();

	//////////////////////////////////////////////////////////////////////////////////
}

void GameApplication::buttonHit(OgreBites::Button* b)
{
	if (b->getName() == "ClickMe")
	{
		//Delete start GUI and start game
		if (!startGame){
			mTrayMgr->hideCursor();
			PlaySound(NULL, NULL, SND_ASYNC);
			mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER); //going to remove
			loadEnv();
			setupEnv();
			startGame = true;
			houseHUD->show();
		}
	}
	else if (b->getName() == "instruct")
	{
		//hide what we dont need
		cont->hide();
		inst->hide();
		cred->hide();

		//set up text
		texty->setCaption("Instructions");
		texty->clearText();
		texty->setText(instruction);
			
		texty->show();
		back->show();
	
	}else if (b->getName() == "credit"){
		//hide shit we dont need
		cont->hide();
		inst->hide();
		cred->hide();

		//set text box ups
		texty->setCaption("Credits");
		texty->clearText();
		texty->setText(credits);

		//show stuff we want
		texty->show();
		back->show();

	}else if (b->getName() == "back"){
		cont->show();
		inst->show();
		cred->show();
		texty->hide();
		back->hide();
	}else if (b->getName() == "retry"){  //this code restarts after endGame deleted everything
		restartLevel();

		PlaySound(music.c_str(), NULL, SND_FILENAME|SND_ASYNC); 

		//loadEnv();
		//setupEnv();
	}else if (b->getName() == "next"){  //this code restarts after endGame deleted everything
		level++;
		nextLevel();

		PlaySound(music.c_str(), NULL, SND_FILENAME|SND_ASYNC); 

		//loadEnv();
		//setupEnv();
	}
}


void GameApplication::endGame(char condition){

	PlaySound(NULL, NULL, NULL);
	gameOver = true;
	startGame = false;
	//Give mouse back
	mTrayMgr->showCursor();

	//Display is different based on if the player won or lost
	if (condition == 'l'){
		mTrayMgr->createLabel(OgreBites::TL_CENTER, "end", "YOU'RE A LOSE!!", 300.0f);
		OgreBites::Button *retry = mTrayMgr->createButton(OgreBites::TL_CENTER, "retry", "Restart?", 200.0f);
		mTrayMgr->buttonHit(retry);
	}else if (condition == 'w'){
		mTrayMgr->createLabel(OgreBites::TL_CENTER, "end2", "YOU ARE WINNER!!", 300.0f);
		OgreBites::Button *next = mTrayMgr->createButton(OgreBites::TL_CENTER, "next", "Next Level?", 200.0f);
		mTrayMgr->buttonHit(next);
	}

}

void GameApplication::destroyallChildren(Ogre::SceneNode* p){
	Ogre::SceneNode::ObjectIterator it = p->getAttachedObjectIterator();
	while (it.hasMoreElements()){
		Ogre::MovableObject* o = static_cast<Ogre::MovableObject*>(it.getNext());
		p->getCreator()->destroyMovableObject(o);
	}

	Ogre::SceneNode::ChildNodeIterator itChild = p->getChildIterator();

   while ( itChild.hasMoreElements() )
   {
      Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
      destroyallChildren( pChildNode );
   }

}

void GameApplication::restartLevel(){
	for(Robot* r : RobotList){
		r->restart();
	}
	yoshPointer->restart();
	houseHealth = 1.0;
	startGame = true;
	houseHealth = 1.0f;
	gameOver = false;
	mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);
	houseHUD->setProgress(houseHealth);
	mTrayMgr->hideCursor();
	
}

void GameApplication::nextLevel(){
	Ogre::Vector3 housePos;
	if (level == 1){
		housePos = grid->getPosition(3, 11);
		housePos[1] = 27;
		housePointer->setPosition(housePos);
		Ogre::Vector3 yoshPos = grid->getPosition(5, 11);
		yoshPointer->setPosition(yoshPos[0], 0, yoshPos[2]);
	}
	else if (level == 2){
		housePos = grid->getPosition(11, 11);
		housePos[1] = 27;
		housePointer->setPosition(housePos);
		Ogre::Vector3 yoshPos = grid->getPosition(13, 11);
		yoshPointer->setPosition(yoshPos[0], 0, yoshPos[2]);
	}
	else{
		int row = rand()%18+1;
		int col = rand()%18+1;
		housePos = grid->getPosition(row, col);
		housePos[1] = 27;
		housePointer->setPosition(housePos);
		Ogre::Vector3 yoshPos = grid->getPosition(row+2, col);
		yoshPointer->setPosition(yoshPos[0], 0, yoshPos[2]);
	}
	Ogre::Vector3 robPos;
	for(Robot* rob:RobotList){
		do {
			rob->restart();
			robPos = grid->getPosition(rand()%20+1, rand()%20+1);
		} while(housePointer->getPosition().distance(robPos) < 80);
		//rob->setInitPos(robPos);
		rob->setPosition(robPos[0], 0, robPos[2]);
	}
	houseHealth = 1.0;
	startGame = true;
	houseHealth = 1.0f;
	gameOver = false;
	mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);
	houseHUD->setProgress(houseHealth);
	mTrayMgr->hideCursor();
}
