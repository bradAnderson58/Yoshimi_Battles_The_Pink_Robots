YOSHIMI BATTLES THE PINK ROBOTS - README

Your name is Yoshimi, and you're a blackbelt in karate

How to play:
	The player controls Yoshimi thusly:
		WASD moves Yoshimi.  W and D move forward and back.  S and D 'strafe' Yoshimi side to side
		Moving the mouse will rotate Yoshimi (These are essentially Skyrim controls)
		Spacebar is for Jumping (This serves no purpose)
		Left click executes Sweet Judo Sword Attack
		Right click executes Ultimate Super Kick

	The pink robots will attempt to attack your house, which you must defend by hitting them with your attacks

	Win Condition:  A level is completed by killing all the robots
	Lose Condition: A level is lost if your house's health reaches 0

How it Meets the Requirements:

	The player interacts with the game world
	There are way more than three game states:
		Robots have multiple game states
		The level itself has game states
		Yoshimi's attacks and movement are state based
	The characters are animated
	Robots spawn randomly
	There are three levels, and level three loops infinitely - so there are technically infinte levels!
	Robots use Boid flocking algorithm to group together for attack formation
	Collision detection: between Yoshimi and Robots, Robots and Robots, Yoshimi and Walls, Robots and Walls, Yoshimi 	attack and Robots
	Physics: Robots get knocked back by attacks, fly through the air, and bounce off walls
	Health bar for house displays state of the house.  GUI pop-ups for win and lose conditions.  Rainbow guns show		which Robot is attacking the house and present time.
	This game is different than anything you have ever seen!
	Game can be fairly challenging in later levels.  I think its fun.
	Got them 60 fps.  And now with comments!
	Pics and Video in folder labeled as such.

Who did what:

	Brad did Yoshimi attacking and movement.  Robot kicking and Rainbow Guns.  GUI stuffs, and other random tasks
	Kevin did Robot flocking, fleeing, seeking, and state management.  Also surrounding of house and other random tasks

	For break down on a per-commit bases, please refer to:
		https://github.com/bradAnderson58/Yoshimi_Battles_The_Pink_Robots/commits/master