#include "GameApplication.h"
#include "Grid.h" // Lecture 5
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
	state = TRAJECTORY;		//first state should be trajectory
	lightColor = WHITE;		//default lighting is white
	pos = true;
	slowMe = 0;			//adjust speed of slider
	lightMe = 0;		//for changing lighting
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
    loadEnv();
	setupEnv();
	loadObjects();
	loadCharacters();
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
	GridNode *temp;  //FOR TESTING REMOVE

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

	Grid grid(mSceneMgr, z, x); // Set up the grid. z is rows, x is columns
	
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
			if (rent != NULL)		// it might not be an agent or object
				if (rent->agent)	// if it is an agent...
				{
					// Use subclasses instead!
					if (c == 'n') {
						agent = new Yoshimi(this->mSceneMgr, getNewName(), rent->filename, rent->y, rent->scale, this);
						yoshPointer = (Yoshimi*) agent;  //you are a yoshimi
					}else {
						agent = new Robot(this->mSceneMgr, getNewName(), rent->filename, rent->y, rent->scale, this);
						agentList.push_back(agent);
					}
					//agent->setApp(this);  //in constructor
					
					agent->setPosition(grid.getPosition(i,j).x, 0, grid.getPosition(i,j).z);
					
				}
				else	// Load objects
				{
					//The temp object holds a pointer to the barrel node, which we need for bounding box access
					temp = grid.loadObject(getNewName(), rent->filename, i, rent->y, j, rent->scale);
					
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
					grid.getNode(i,j)->setOccupied();  // indicate that agents can't pass through
					mNode->setPosition(grid.getPosition(i,j).x, 10.0f, grid.getPosition(i,j).z);
				}
				else if (c == 'e')
				{
					ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
					ParticleSystem* ps = mSceneMgr->createParticleSystem(getNewName(), "Examples/PurpleFountain");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ps);
					mNode->setPosition(grid.getPosition(i,j).x, 0.0f, grid.getPosition(i,j).z);
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
	grid.printToFile(); // see what the initial grid looks like.

	//boundBox = temp->entity->getWorldBoundingBox(true);  //boundBox is the bounding box for the barrel
	
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
	mLight = mSceneMgr->createLight();
	mLight->setType(Light::LT_POINT);
	mLight->setPosition(-10, 40, 20);
	mLight->setSpecularColour(ColourValue::White);
	mLight->setDiffuseColour(ColourValue::White);

	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8); // pretty sky
}

void // Load other props or objects
GameApplication::loadObjects()
{

}

void // Load actors, agents, characters
GameApplication::loadCharacters()
{
	// Lecture 5: now loading from file
	// agent = new Agent(this->mSceneMgr, "Sinbad", "Sinbad.mesh");
}

void
GameApplication::addTime(Ogre::Real deltaTime)
{
	// Iterate over the list of agents (robots)
	std::list<Agent*>::iterator iter;
	for (iter = agentList.begin(); iter != agentList.end(); iter++)
		if (*iter != NULL)
			(*iter)->update(deltaTime);

	yoshPointer->update(deltaTime); //Yoshimi has a different update function
	

	if (state != FIRE && state != ANIMATE && slowMe >= 3){

		//If the slider is at the top or bottom, reverse it.
		Ogre::Real val = mSlider->getValue();
		if (val >= 100) pos = false;
		else if (val <= 0) pos = true;

		//move the slider
		if (pos) mSlider->setValue(val + 1);
		else mSlider->setValue(val - 1);
		slowMe = 0;
		
		//reset color value
		if (lightColor != WHITE){
			lightMe++;						//leave different color up for a few frames
			if (lightMe > 5){
				changeLighting(WHITE);
				lightMe = 0;
			}
		}
	}
	slowMe++;
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
		//States!  This is the case where the player must choose a trajectory
		if (state == TRAJECTORY) {
			//updte trajectory
			trajVal = (mSlider->getValue());

			//uncomment this to test success condition with hardcoded value:
			//trajVal = 35;

			mSlider->setCaption("Velocity");
			mSlider->setValue(0);
			state = VELOCITY;

		//This is the case where the player must choose a velocity
		}else if (state == VELOCITY){
			//update velocity val
			velVal = (mSlider->getValue());

			//uncomment this to test success condition with hardcoded value:
			//velVal = 40;

			mSlider->setCaption("FIRE FISH!");
			mSlider->setValue(0);
			state = FIRE;

		//This is the state where the fish fires (The fish will reset to trajectory state thru a pointer after animation completes)
		}else if (state == FIRE){
			this->agent->fire(trajVal, velVal); // rocket propelled fish!
			state = ANIMATE;
			
			//mSlider->setCaption("Trajectory");
		}else{ std::cout << "Yo you gotta wait!" << std::endl; }
	}
	else if (arg.key == OIS::KC_W) {
		
		yoshPointer->setMovement('f', true);
	}
	else if (arg.key == OIS::KC_A) {
		yoshPointer->setMovement('l', true);
	}
	else if (arg.key == OIS::KC_D) {
		yoshPointer->setMovement('r', true);
	
	}
	else if (arg.key == OIS::KC_S) {
		yoshPointer->setMovement('b', true);
	}
   
    //mCameraMan->injectKeyDown(arg);
    return true;
}

bool GameApplication::keyReleased( const OIS::KeyEvent &arg )
{
	//Set the flag to false for whichever key is no longer pressed
	if (arg.key == OIS::KC_W) yoshPointer->setMovement('f', false);
	else if (arg.key == OIS::KC_A) yoshPointer->setMovement('l', false);
	else if (arg.key == OIS::KC_S) yoshPointer->setMovement('b', false);
	else if (arg.key == OIS::KC_D) yoshPointer->setMovement('r', false);

    //mCameraMan->injectKeyUp(arg);
    return true;
}

bool GameApplication::mouseMoved( const OIS::MouseEvent &arg )
{
	//std::cout << arg.state.X << std::endl;
	//std::cout << arg.state.Y << std::endl;

	yoshPointer->rotationCode(arg);
	
    //if (mTrayMgr->injectMouseMove(arg)) return true;
    //mCameraMan->injectMouseMove(arg);
    return true;
}

bool GameApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	//////////////////////////////////////////////////////////////////////////////////////
	// Lecture 12
	if(id == OIS::MB_Left)
	{
		//show that the current object has been deselected by removing the bounding box visual
		if(mCurrentObject)
		{
			mCurrentObject->showBoundingBox(false);
		}
 
		//find the current mouse position
		Ogre::Vector3 mousePos;
		mousePos.x = arg.state.X.abs;
		mousePos.y = arg.state.Y.abs;
		mousePos.z = arg.state.Z.abs;
		
 
		//then send a raycast straight out from the camera at the mouse's position
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.x/float(arg.state.width), mousePos.y/float(arg.state.height));
		mRayScnQuery->setRay(mouseRay);
		mRayScnQuery->setSortByDistance(true);
		
		/* This next chunk finds the results of the raycast */
		Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
		Ogre::RaySceneQueryResult::iterator iter = result.begin();
 
		for(iter; iter != result.end(); iter++)
		{
			if(iter->movable && iter->movable->getName().substr(0, 5) != "tile[")
			{
				mCurrentObject = iter->movable->getParentSceneNode();
				break;
			}
		}
 
		//now we show the bounding box so the user can see that this object is selected
		if(mCurrentObject)
		{
			mCurrentObject->showBoundingBox(true);
		}
 
		bLMouseDown = true;
	}
	else if (id == OIS::MB_Right)
		bRMouseDown = true;

	//////////////////////////////////////////////////////////////////////////////////////
   
    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    mCameraMan->injectMouseDown(arg, id);
    return true;
}

bool GameApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	// Lecture 12
	if(id  == OIS::MB_Left)
	{
		bLMouseDown = false;
	}
	if(id  == OIS::MB_Right)
	{
		bRMouseDown = false;
	}
	else
		mCameraMan->injectMouseUp(arg, id);

	//////////////////////////////////////////////////////////////////////////////
   
    if (mTrayMgr->injectMouseUp(arg, id)) return true;
    mCameraMan->injectMouseUp(arg, id);
    return true;
}

void GameApplication::createGUI(void)
{
	//////////////////////////////////////////////////////////////////////////////////
	
	if (mTrayMgr == NULL) return;
	using namespace OgreBites;

	std::string label = "Trajectory"; 
	
	//The slider will be used to get the value of the trajectory and the velocity
	mSlider = mTrayMgr->createThickSlider(TL_TOP, "SampleSlider", label, 250, 80, 0, 0, 0);
	mSlider->setRange(0, 100, 101); //so the third parmeter for this method was not what i was expecting: much confuse
	mSlider->setValue(50);
	mTrayMgr->sliderMoved(mSlider);
	
	//use the paramsPanel to display number of successful shots
	Ogre::StringVector items;
	items.push_back("Successes");
	mParamsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_TOPRIGHT,"Testing",250,items);
	mParamsPanel->setParamValue(0, Ogre::StringConverter::toString(0));

	mTrayMgr->showAll();

	//////////////////////////////////////////////////////////////////////////////////
}

void GameApplication::setSuccess(int num){
	mParamsPanel->setParamValue(0, Ogre::StringConverter::toString(num));
}

void GameApplication::changeLighting(colorVal change){
	lightColor = change;
	if (lightColor == WHITE){
		// change ambient and specular to white
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
		mLight->setSpecularColour(Ogre::ColourValue::White);
		mLight->setDiffuseColour(Ogre::ColourValue::White);
	
	//change the scene red
	}else if (lightColor == RED){
		mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 0.1f, 0.1f));
		mLight->setSpecularColour(Ogre::ColourValue::Red);
		mLight->setDiffuseColour(Ogre::ColourValue::Red);
	
	//change the scene green
	}else if (lightColor == GREEN){
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 1.0f, 0.1f));
		mLight->setSpecularColour(Ogre::ColourValue::Green);
		mLight->setDiffuseColour(Ogre::ColourValue::Green);
		
	//error checking
	}else{
		std::cout << "No valid color" << std::endl;
	}
}