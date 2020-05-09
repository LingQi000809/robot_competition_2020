
# Milestone 5
## Solve a single maze with branching.

### Arena Setup
![arena5](arena5.jpg)



### Strategy / Logic
#### 1. Types of Intersections
- Left Turn  
![leftTurn](leftTurn.jpeg)
When the line is under the leftmost sensors of the robot, the robot is considered to encounter a left turn.  

- Right Turn  
![rightTurn](rightTurn.jpeg)
When the line is under the rightmost sensors of the robot, the robot is considered to encounter a right turn.  

- T Intersection  
![tIntersection](tIntersection.jpeg)
When the line is under every sensor of the robot, the robot is considered to encounter a T intersection.  
**The end of the maze is also considered a T intersection.**

- Dead End  
![deadend](deadend.jpeg)
When the line ends without any intersection, the robot is considered to encounter a dead end.

#### 2. Conditions and Actions
![venn](venn.jpeg)
As the venn diagram shows, the conditions for t-intersections include the ones for left turns and right turns. Thus, the t-intersection condition must be examined prior to the ones for left and right turns. Otherwise, a t-intersection will never be detected.   
  
The robot follows this logic to execute actions:  
![logic](Logic.jpg)


### Problems Encountered

#### 1. super slow turns in the dead-end
From time to time, the robot ran into a slow anti-clockwise rotate at a dead-end. Originally, I thought that something was wrong with my left motor, because it only had this behavior anti-clockwise. However, later I discovered that this was due to a line of code: 
> v = error / LINEPOS_SPEED_RATIO + NORMAL_SPEED;
This was the code to adjust the speed in the follow-line behavior. The problem was that **the error could be negative** based on the following calculation, and the error would be negative only when the line is to the left of the robot - This explains why the weird behavior only happened when turning to the left (anti-clockwise).    
> linePos = sensors.readLine(sensorValues);  
> error = linePos - 2500;   
Thus, I changed the code to the absolute value of error:  
> v = abs(error) / LINEPOS_SPEED_RATIO + NORMAL_SPEED;  
The problem is solved.


#### 2. half-turn vs. full-turn

#### 3. detecting T intersection


  
### MILESTONE 5 COMPLETED!   
[![milestone4-video](http://img.youtube.com/vi/qSIA5p916dI/0.jpg)](https://www.youtube.com/watch?v=qSIA5p916dI)
*(This is a Youtube link)*  

