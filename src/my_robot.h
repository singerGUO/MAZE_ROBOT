#ifndef __ROBOT_AGENT__H
#define __ROBOT_AGENT__H 

#include <string>
#include <math.h>
#include "enviro.h"

double angle_cnt = 0;
double angle_cnt_target =0;
namespace {

	using namespace enviro;

	
	//state find the goal
	class Goal : public State, public AgentInterface
    {
    public:
        void entry(const Event &e) {
            std:: cout << "Entering Goal\n";
			track_velocity(0, 0);
        }      
        void during(){}  
        void exit(const Event &e) {}
    };
	// adjust the angle until the object direction is heading to target directly
	class Adjusting : public State, public AgentInterface
    {
    public:
        void entry(const Event &e) {
            std:: cout << "Entering Adjusting angle to find the object\n";
        }
        // acquire a lock that
        void during(){
			if (flag == 0) {
				flag = 1;
				angle_cnt_target += 1.5708;
			}
			if (angle() <= angle_cnt_target) {
				track_velocity(0, 1.5708);
			}
			std:: cout << angle()<<"\n";
			std:: cout << angle_cnt_target<<"\n";

			if (angle() >= angle_cnt_target) {
				auto pos_agent = position();
				while (angle() != angle_cnt_target)
				{
					teleport(pos_agent.x, pos_agent.y, angle_cnt_target);
				}
				flag = 0;
				if (sensor_reflection_type(0) == "Target"){
					angle_cnt_target = 0;
					emit(Event("tick"));
				}
			}
		}  
        void exit(const Event &e) {}
       	private:
		int flag = 0;
    };
	// this is the state to moving forward and in the meanwhile detecting target and wall
	class MovingForward : public State, public AgentInterface {
	public:

		// get the  current angle of the agent 
		void entry(const Event& e) {
			angle_cnt = angle();
		}

		// moves agent with constant velocity until it crash or any of sensor find the target
		void during() {
			std::cout << "head sensor dist:" << sensor_value(0) << std::endl;
			track_velocity(20, 0);
			if (sensor_value(0) < 22) {
				if (sensor_reflection_type(0) == "Target"){
					emit(Event("goal"));
				}
                std::cout << "movingforward to rotate state change since crash" << std::endl;
				emit(Event("tick"));
            }//Check all sensors find the target
			else if(sensor_reflection_type(0) == "Target" ||sensor_reflection_type(1) == "Target"||sensor_reflection_type(2) == "Target"|| sensor_reflection_type(3) == "Target"){ 
                track_velocity(0,0);
				//if it is pointed to target
				if (sensor_reflection_type(0) == "Target"){
					track_velocity(20, 0);
				}else{
					//it is not adjusted to target yet continue rotate
					std::cout << "movingforward to rotate state change since adjust angle to target" << std::endl;
					emit(Event("adjust"));
				}

			}
		
		}
	
		void exit(const Event& e) {}
	}; 

	//This state is used for rotating while wandering, before crashing move to open space compared with the left and right sensors
	class Rotating : public State, public AgentInterface {
	public:
		void entry(const Event& e) { }

		void during() {
			if (flag == 0) {
				flag = 1;
				if (sensor_value(1) > sensor_value(3)) {
					dir = 0;
					angle_cnt += 1.5708;
				}
				else {
					dir = 1;
					angle_cnt -= 1.5708;
				}
			}
			if ((dir == 0) && (angle() <= angle_cnt)) {
				track_velocity(0, 1.5708);
			}
			else if ( (angle() >= angle_cnt && (dir == 1))) {
				track_velocity(0, -1.5708);
			}
			if ((dir == 0 && angle() >= angle_cnt) || (dir == 1 && angle() <= angle_cnt)) {
				auto pos = position();
				while (angle() != angle_cnt)
				{
					teleport(pos.x, pos.y, angle_cnt);
				}
				std::cout << "rotate to movingforward state change" << std::endl;
				flag = 0;
				emit(Event("tick"));
			}

		}
		void exit(const Event& e) {}

	private:

		int flag = 0, dir = 0;
	};
	// Robot state machine
	class RobotController : public StateMachine, public AgentInterface {

	public:
		// Constructor to define the states transistions
		RobotController() : StateMachine() {
			set_initial(moving_forward);
			add_transition("tick", moving_forward, rotating);
			add_transition("tick", rotating, moving_forward);
			add_transition("tick", adjust_target_angle, moving_forward);
			add_transition("adjust", moving_forward, adjust_target_angle);
			add_transition("goal", moving_forward, goal);

		}

		MovingForward moving_forward;
		Rotating rotating;
		Goal goal;
		Adjusting adjust_target_angle;
	};

	//! The  controller agent 
	class Controller : public Agent {

	public:
		//! Constructor adds a process
		Controller(json spec, World& world) : Agent(spec, world) {
			add_process(c);
		}

		RobotController c;

	};

	DECLARE_INTERFACE(Controller);

}

#endif