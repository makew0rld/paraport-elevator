/*
    Written by XXXXXXX (makeworld-the-better-one) on June 14th, 2019.
    Help was provided by Tanmay Patel and William.
    Compiled on Fedora Linux with gcc, using the parallel port usually reserved for printers.
    This code completes the requirements of the TEJ2O summative assignment, which requires
    a working elevator to be built that can stop at multiple floors, and be controlled by a
    motor.
    Note that this code still contains debug messages, to help any other users. Its binary
    must also be run with superuser permissions (sudo) because it accesses a port.

    This is free software released into the public domain. No copyright is applied
    For more information, please refer to
    <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
*/


#include <stdio.h>
#include <unistd.h>
#include </usr/include/sys/io.h>

#define BASEPORT 0x378  // Parallel port address

int main() {
    ioperm(BASEPORT, 3, 1);  // Get permission to access the port
    int floor;  // The desired floor to go to
	int cur_floor = 1;

    /*
        The following arrays represent the time to wait between each floor. 
        The first variable is for the time it takes going forward (clockwise),
        and the second is going backward (counter-clockwise).
        The numbers are the time from floor 1-2, then 2-3, then 3-4
        Note that the time is in microseconds, but written in seconds*1000000 for readability.
    */
    // XXX: These values will need to be tuned for different motors, weights, and even
    //      if the motor's connection to the shaft is adjusted.
	double dist_forward_times[] = {0.47*1000000, 0.5*1000000, 0.4*1000000};
	double dist_back_times[] = {0.4*1000000, 0.5*1000000, 0.5*1000000};

    // These values can be easily changed depending on the motor and bridge setup
	int right = 1;  // Output to port to go counter-clockwise
	int left = 2;  // Output to port to go clockwise
    
    outb(0, BASEPORT);
    printf("Welcome to the elevator.\n");
    printf("\nEnter the floor the elevator is currently at. The default is 1.\n");
    // Continually ask for the starting floor number until it is within the valid range
    printf("> ");
    do {
        // -38 means newline because of int conversion below
        if (cur_floor == -38) {
            printf("> ");
        }
        cur_floor = getchar()-'0';  // Subtracting '0' converts their entry to an int, due to binary ASCII values

        if ((cur_floor < 1 || cur_floor > 4) && cur_floor != -38) {
            printf("Invalid starting floor number.\n");
        }
    } while (cur_floor < 1 || cur_floor > 4);
    
    printf("Type a floor number you'd like to go to (1-4), or X to quit.\n");
    while (floor != 72 && floor != 40) {  // 'x' or 'X' -- this is because of the conversion to int above
        if (floor == -38) {  // Newline
            // Print out another prompt when a newline is entered
            // This means a prompt will be printed when nothing is entered,
            // or when anything is typed.
            printf("> ");
        }
        // Get the desired floor repeatedly in the loop
        floor = getchar()-'0';  // Convert to int
        
        // Special chars
        if (floor == -38) {  // Newline
            // If they hit enter, it just goes back to the top of the loop,
            // displaying the prompt again
            continue;
        }
        // These chars are for quitting
        if (floor == 72) {  // 'x'
            break;
        }
        if (floor == 40) {  // 'X'
            break;
        }
        
        printf("Floor entered: %i\n", floor);  // Debug message
        if (floor < 1 || floor > 4) {
            // This also catches non-numbers, since they have been
            // converted to int above
            printf("Invalid floor. You must enter a number from 1 to 4.\n");
            continue;
        }
        
        // Go to the specified floor
        if (cur_floor < floor) {  // Need to move to the right
            outb(right, BASEPORT);
            // Iterate through dist_forward_times to sleep for the distance between each floor
            // This allows skipping through floors, by sleeping for all the distances between
            // the current floor and the desired one
            for (int i = cur_floor-1; i < floor-1; i++) {
                printf("*** Going right, i=%i\n", i);  // Debug message
                usleep(dist_forward_times[i]);
            }
            outb(0, BASEPORT);
            cur_floor = floor;  // We've arrived at the floor
        } else if (cur_floor > floor) {  // Need to move to the left
            outb(left, BASEPORT);
            // Same iteration as outline above, but in reverse order,
            // and using dist_back_times instead
            for (int i = cur_floor-2; i > floor-2; i--) {
                printf("*** Going left, i=%i\n", i);  // Debug message
                usleep(dist_back_times[i]);
            }
            outb(0, BASEPORT);
            cur_floor = floor;  // We've arrived at the floor
        } else if (cur_floor == floor) {
            printf("You are already on that floor.\n");
        } else {
            printf("*** Unknown situation.\n");  // Debug message, just in case
        }
        
    }
    
    outb(0, BASEPORT);
    ioperm(BASEPORT, 3, 0);  // Release the port

    return 0;
}
