FISH IN A BARREL - README

Fish in a barrel is a game where you shoot fish into a barrel.

How to play:
	The player will notice a slider at the top of the screen labelled 'Trajectory' which is sliding back and forth,
	 this is the angle the fish will be fired at.  When the player hits spacebar, they set the angle for the fish.

	After selecting the angle, the slider label will change to 'Velocity', and the player will now select the velocity
	 of the fish.  As before, pressing spacebar will select the current value on the slider

	Now that trajectory and velocity have been established, it is time to fire the fish.  Again, pressing spacebar.

	If the fish hits the barrel, the success counter in the upper right will be incremented, and the player will also
	 be rewarded will a flash of green on the screen.  Missing produces a flash of red.  The player may continue firing
	 fish for as long as their patience allows.

	Win Condition:  Get more successes than your friends.

How to Test:

	You can test it by playing.  For testers who may have issues hitting the barrel (and thus testing success), there
	 are two lines which can be uncommented in the spacebar handler which will automatically set the velocity and
	 trajectory values to a successful fish firing, regardless of when they hit the spacebar.
	
	To view bounding boxes, there is also a line in the Grid loadObject method to display barrel boundingBox and
	 a line in the shoot method of Agent to see boundingBox of the fish.

	All the camera movement code has been removed for simplicity.  The player cannot move the camera.