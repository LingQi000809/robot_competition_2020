
# Milestone 3
## Follow A Line along a 2'*2' square.

### ATTEMPT 1
- I used the code for Milestone 2.

##### PROBLEMATIC OUTCOMES: 
- The robot is moving bang-bang-ly again.
- The robot stops at the corner of the square!

### ATTEMPT 2

**BANG-BANG**
- I increased the error threshold from 500 to 1000, which meant that the speed only changed when the reading of the line was smaller than 1500 or larger than 3500.  

**TURN AT THE CORNER**  
I had several ideas to deal with this one:  
1) A **timer** recording the duration of the error being 2500.  
  The problem is that it's hard to know how long the robot takes to make a turn. So it's hard to tell whether the robot is encountering a turn or the exit of a line. Also, in a maze, the robot can go into a **dead-end** and the timer might mislead it to stop though it should turn around.
2) Calculate the **angle** and make a 90 degree turn.  
  The problem is that it's hard to figure out whether the robot should turn clockwise or anti-clockwise.
3) Don't stop based on the error reading! Just keep turning.
  I checked the example image of a maze, and found out that there will be a special indication when it's the exit of the maze. Thus, I don't need to program a stopping point when the robot comes to an end of a line.
  - **When the error is 2500, the motors turn in opposite maximum speed, which makes a sharp turn.**

### MILESTONE 3 COMPLETED!   
[![milestone3-video](http://img.youtube.com/vi/fUd89y8w_i4/0.jpg)](https://www.youtube.com/watch?v=fUd89y8w_i4)
*(This is a Youtube link)*  

The robot seems to be moving at different speeds on different sides of the square. This is probably because the speed when the robot is moving in an optimal range is not constant - it depends on the last calculated speed (see milestone2).
