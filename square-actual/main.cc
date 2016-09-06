#include <iostream>
#include <stdio.h>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

int
main(int argc, char *argv[])
{
	try
	{
		PlayerClient robot("localhost", 6665);
		Position2dProxy pp(&robot, 0);

        for (int i = 0; i < 4; i++) {

          // KØR FREMAD!
          double move_speed = 0.2;
          timespec move_sleep = {5, 0};
          double move_turn_rate = 0.0;
	
          pp.SetSpeed(move_speed, DTOR(move_turn_rate));
          nanosleep(&move_sleep, NULL);

          // DREJ!
          double turn_speed = 0.0;
          timespec turn_sleep = {6, 0};
          double turn_turn_rate = 15.0;

          pp.SetSpeed(turn_speed, DTOR(turn_turn_rate));
          nanosleep(&turn_sleep, NULL);

        }

		// STOP!
        timespec stop_sleep = {1, 0};
		pp.SetSpeed(0, 0);
		nanosleep(&stop_sleep, NULL);
        
	} //end try
	catch (PlayerCc::PlayerError e)
	{
		std::cerr << e << std::endl;
		return -1;
	}
}
