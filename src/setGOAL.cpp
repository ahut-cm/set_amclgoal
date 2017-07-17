    #include <ros/ros.h>  
    #include <move_base_msgs/MoveBaseAction.h>  
    #include <actionlib/client/simple_action_client.h>  
    #include "geometry_msgs/PoseWithCovarianceStamped.h"    
    #include "std_msgs/String.h"
    #include <sstream>
    #include "set_amclgoal/amcl_type.h"
    typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;  
typedef struct _POSE
{
  double X;
  double Y;
  double Z;
  double or_x;
  double or_y;
  double or_z;
  double or_w;
} POSE;


using namespace std;

#define SETAMCL "SETAMCL"
bool Callback_flag = false;
bool SetGoal_flag = false;
bool goal_reach_flag = false;
string msg_str = "";
POSE set_GOAL_java;

void poseCallback(const std_msgs::String::ConstPtr &msg)
{
     ROS_INFO_STREAM("Topic is Subscriber ");
     std::cout<<"get topic text: " << msg->data << std::endl;
      
     Callback_flag = true;
     msg_str = msg->data;
}  

void GoalCallback(const set_amclgoal::amcl_type::ConstPtr &msg)
{
    //set_amcl_java.X = msg.A;
    set_GOAL_java.X = msg->X;
    set_GOAL_java.Y = msg->Y;
    set_GOAL_java.Z = msg->Z;
    set_GOAL_java.or_x = msg->OR_X;
    set_GOAL_java.or_y = msg->OR_Y;
    set_GOAL_java.or_z = msg->OR_Z;
    set_GOAL_java.or_w = msg->OR_W;
    SetGoal_flag = true;
}  


void setGoal(POSE pose)
{
     //tell the action client that we want to spin a thread by default 
    MoveBaseClient ac("move_base", true); 
       
    //wait for the action server to come up 
    while(!ac.waitForServer(ros::Duration(5.0))){ 
        ROS_WARN("Waiting for the move_base action server to come up"); 
    } 
       
    move_base_msgs::MoveBaseGoal goal; 
       
    //we'll send a goal to the robot to move 1 meter forward 
    goal.target_pose.header.frame_id = "map"; 
    goal.target_pose.header.stamp = ros::Time::now(); 
    
    goal.target_pose.pose.position.x = pose.X;
    goal.target_pose.pose.position.y = pose.Y; 
    goal.target_pose.pose.position.z = pose.Z;  
    goal.target_pose.pose.orientation.x = pose.or_x ;
    goal.target_pose.pose.orientation.y = pose.or_y ;
    goal.target_pose.pose.orientation.z = pose.or_z ;
    goal.target_pose.pose.orientation.w = pose.or_w ;  
     
    ROS_INFO("Sending goal"); 
     
    ac.sendGoal(goal); 
       
    ac.waitForResult(); 
       
    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) 
      
      {
          ROS_INFO("it is successful"); 
          goal_reach_flag = true;
          
      }
     else 
       ROS_ERROR("The base failed  move to goal!!!");  
}

int main(int argc, char** argv){  
    ros::init(argc, argv, "base_amcl_set"); 
    ros::NodeHandle nh;
    ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 10);
    ros::Subscriber sub = nh.subscribe("/base/amcl_set",10,poseCallback);
    ros::Subscriber sub2 = nh.subscribe("/base/goal_set_point",10,GoalCallback);
    ros::Publisher xfer_pub = nh.advertise<std_msgs::String>("xfwords", 10);
      //tell the action client that we want to spin a thread by default  
      MoveBaseClient ac("move_base", true);  
      
      //wait for the action server to come up  
  /*    while(!ac.waitForServer(ros::Duration(5.0))){  
        ROS_INFO("Waiting for the move_base action server to come up");
      }    */
     // POSE pose_AMCL = {0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0};

     POSE amcl_point_launch;
     ros::param::get("~GOAL_Point_x",set_GOAL_java.X);
     ros::param::get("~GOAL_Point_y",set_GOAL_java.Y);
     ros::param::get("~GOAL_Point_z",set_GOAL_java.Z);
     ros::param::get("~GOAL_orientation_x",set_GOAL_java.or_x);
     ros::param::get("~GOAL_orientation_y",set_GOAL_java.or_y);
     ros::param::get("~GOAL_orientation_z",set_GOAL_java.or_z);
     ros::param::get("~GOAL_orientation_w",set_GOAL_java.or_w);
     

 while(ros::ok())
  {

       if(SetGoal_flag == true)
      {
        SetGoal_flag = false;
 
            setGoal(set_GOAL_java);
            ROS_INFO_STREAM("SetAmcl succeed ");
              std_msgs::String msg,msg2;  
    std::stringstream ss;  
    std::stringstream ss2;  
    ss << "设置GOAL成功，正在去往目的地";  
    ss2 << "到达目的地,导航结束";  
    msg.data = ss.str(); 
    msg2.data = ss2.str(); 
    ROS_INFO("%s", msg.data.c_str());  
    xfer_pub.publish(msg);  
    if(goal_reach_flag == true)
    {
        goal_reach_flag = false;
        xfer_pub.publish(msg2);
    }
     ros::spinOnce();
            
      }

        else
        {
 
        }
      
 
      ros::spinOnce();
     // rate_loop.sleep();
  }
   
 
  return 0; 
}  