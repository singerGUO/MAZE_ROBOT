# MAZE_ROBOT
EEP520 final project

## Overview

This is the final project that simulate a multi-agent systems that robot find out a target in the complicated maze with **ELMA** and **ENVIRO** 

# Installation

## Clone project

```
git clone https://github.com/singerGUO/MAZE_ROBOT.git
```

## Start the docker image with ENVIRO

```bash
docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:v1.61 bash
esm start
```

## key challenges and how they were addressed

There are two key challenges:

#### Four way Sensors to find target

To add the efficiency to find the target, Object has four-way sensors to detect the goal. However, it will be more complicated to add more state like go up, go down, go left and go right. while  there is only ways movement for oriented objects. 

I addressed this problem by using creating a state called the Adjusting to adjust the sensor[0]  angle  (for 4 directions clockwise) until it directly point to the target. Therefore, I can use move_forward method to achieve my goal. 

#### Angle Rotation

After rotating the agent, there were time delays that caused the final angle to be slightly different from multiples of 1.57radians (90 degrees). To address this problem, the code checks the angle_cnt  and angle_cnt_adjust global variable to obtain the accurate angle. The teleport() function is then used to properly orient the agent and remove any discrepancies. The position() method is employed to retrieve the x and y coordinates of the final position.

## Run the project 

Inside the src/  folder, make the project and start the enviro server as follows.

```bash
make
enviro
```

Then navigate to `http://localhost` you should see the simulation.

You can press `Ctrl-C` to stop the enviro server.

I was using one sensors to detect wall distance for preventing collisions.

## References

This project take reference from examples in the Enviro Github(https://github.com/klavinslab/enviro).

# License

This software is open source and uses the MIT license. Details can be found [here](https://github.com/klavinslab/elma).
