
# MILESTONE 7
## Have a final run under 15 seconds.

This milestone was quite simple - adjusting the variables to find the right threshold for the increased speed.
Yet, it took long to figure the right fits out. (And here we realized how important soft-coding is!)

> int NORMAL_SPEED = 100; // The base speed of the robot
> int MAX_SPEED = 150; // The Maximum speed (turning speed) of the robot. Also used to calculate the follow-line speed.
> int ADJUSTING_SPEED = 120; // See below
> int moveForwardDelayTime = 120; // See below
> int turnDelayTime = 200; // See below

I had a moveForward function for my robot. This gave the robot some time to perform simple moving forward action without entering any conditions. This allowed it to ignore the intersections during the small amount of time.  
This function was called:
- in the final run, when the robot's action is to move forward (not going into a dead-end);
- after detecting a T-intersection - If it still detected a T-intersection after moving-forward, the robot was considered reaching the end;
- after detecting a left/right turn and before executing the turn - To place the entire robot above the turning point (because the sensor is in the very front of it). Otherwise, when the robot rotates, it will miss the line;
- before turning round when encountering a dead end - To place the entire robot be above the ending point, so that it could turn fully.

Thus, the ADJUSTING_SPEED is the speed at which the robot moves forward. The moveForwardDelayTime is the duration of which the robot moves forward.   
The turnDelayTime is for a similar purpose. When the robot is turning, there is a duration assigned during which the robot ignores the other conditions to avoid having it fall into the follow-line condition.  


### MILESTONE 7 COMPLETED!  
[![milestone7-video](http://img.youtube.com/vi/2HWgPLfeTW0/0.jpg)](https://www.youtube.com/watch?v=2HWgPLfeTW0)
*(This is a Youtube link)*  
 
