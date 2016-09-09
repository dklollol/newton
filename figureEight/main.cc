#include <iostream>
#include <stdio.h>
#include <libplayerc++/playerc++.h>
#include <args.h>
using namespace PlayerCc;

int
main(int argc, char *argv[])
{
	// Parse input arguments
	parse_args(argc,argv);

	try
	{
		PlayerClient robot(gHostname, gPort);
		Position2dProxy pp(&robot, gIndex);

		// Speed and turn settings
		double turn_rate = 15;
		double move_speed = 0.20;
		
		timespec move_sleep = { 24, 4 };
		timespec turn_sleep = { 3, 0 };
		timespec stop_sleep = { 10, 0 };
	
		// move 1, turn 1
		pp.SetSpeed(move_speed, DTOR(turn_rate));
		nanosleep(&move_sleep, NULL);
		pp.SetSpeed(move_speed, DTOR(-turn_rate));
		nanosleep(&move_sleep, NULL);


	
		// Set motor stop command and wait so they can propagate
		pp.SetSpeed(0, 0);
		nanosleep(&stop_sleep, NULL);
	} //end try
	catch (PlayerCc::PlayerError e)
	{
		std::cerr << e << std::endl;
		return -1;
	}
}
