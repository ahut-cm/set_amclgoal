#include <ros/ros.h> 
#include <move_base_msgs/MoveBaseAction.h> 
#include <actionlib/client/simple_action_client.h> 
#include "geometry_msgs/PoseWithCovarianceStamped.h"    
#include "std_msgs/String.h"
 
using namespace std;
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient; 
 
#define SETAMCL "SETAMCL"
#define GODRAMING "DRAMING"
 
bool Callback_flag = false;
string msg_str = "";

/*******************************默认amcl初始点******************************************/
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



 
void setHome( ros::Publisher pub,POSE pose)
{
    geometry_msgs::PoseWithCovarianceStamped msg_poseinit;
    msg_poseinit.header.frame_id = "map";
    msg_poseinit.header.stamp = ros::Time::now();
  /*  nh.param<double>("amcl_pose_position.x", msg_poseinit.pose.pose.position.x,1.959);
    nh.param<double>("amcl_pose_position.y", msg_poseinit.pose.pose.position.y,3.170);
    nh.param<double>("amcl_pose_position.z", msg_poseinit.pose.pose.position.z,0);
    nh.param<double>("amcl_pose_orientation.x", msg_poseinit.pose.pose.orientation.x,0.0);
    nh.param<double>("amcl_pose_orientation.y", msg_poseinit.pose.pose.orientation.y,0.0);
    nh.param<double>("amcl_pose_orientation.z", msg_poseinit.pose.pose.orientation.z,0.961);
    nh.param<double>("amcl_pose_orientation.w", msg_poseinit.pose.pose.orientation.w,-0.278); */
    msg_poseinit.pose.pose.position.x = pose.X;
    msg_poseinit.pose.pose.position.y = pose.Y;
    msg_poseinit.pose.pose.position.z = pose.Z;
    msg_poseinit.pose.pose.orientation.x = pose.or_x;
    msg_poseinit.pose.pose.orientation.y = pose.or_y;
    msg_poseinit.pose.pose.orientation.z = pose.or_z;
    msg_poseinit.pose.pose.orientation.w = pose.or_w;
//因为ros话题原理本身的问题，Setting pose 需要按照以下发送
    pub.publish(msg_poseinit);
    ros::Duration(1.0).sleep();
    pub.publish(msg_poseinit);
    ros::Duration(1.0).sleep();
    pub.publish(msg_poseinit);
    ros::Duration(1.0).sleep();
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
      ROS_INFO("it is successful"); 
     else 
       ROS_ERROR("The base failed  move to goal!!!");  
}
 
void poseCallback(const std_msgs::String::ConstPtr &msg)
{
     ROS_INFO_STREAM("Topic is Subscriber ");
     std::cout<<"get topic text: " << msg->data << std::endl;
      
     Callback_flag = true;
     msg_str = msg->data;
}  
 
int main(int argc, char** argv)
{ 
 ros::init(argc, argv, "base_set_amcl"); 
 ros::NodeHandle nh;
  ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 10);
  ros::Subscriber sub = nh.subscribe("/base/set_amcl",10,poseCallback);
  ros::Publisher xfer_pub = nh.advertise<std_msgs::String>("xfwords", 10);
  //ros::Rate rate_loop(10);
  POSE pose2 = {-8.15833854675, 3.15512728691, 0.0,  0.0, 0.0, -0.740479961141, 0.672078438241};
  POSE pose1 = {2.213, 0.997, 0.0,  0.0, 0.0, 0.976, 0.217};
  POSE pose3 = {0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0};
  POSE pose4 = {0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0};

 /* double pose_AMCL_x,pose_AMCL_y,pose_AMCL_z,pose_AMCL_or_x,
 pose_AMCL_or_y,pose_AMCL_or_z,pose_AMCL_or_w;
 double pose_goal_x,pose_goal_y,pose_goal_z,pose_goal_or_x,
 pose_goal_or_y,pose_goal_or_z,pose_goal_or_w; */

     POSE amcl_point;
     ros::param::get("~AMCL_Point_x",amcl_point.X);
     ros::param::get("~AMCL_Point_y",amcl_point.Y);
     ros::param::get("~AMCL_Point_z",amcl_point.Z);
     ros::param::get("~AMCL_orientation_x",amcl_point.or_x);
     ros::param::get("~AMCL_orientation_y",amcl_point.or_y);
     ros::param::get("~AMCL_orientation_z",amcl_point.or_z);
     ros::param::get("~AMCL_orientation_w",amcl_point.or_w);
     
 
 /* POSE pose_AMCL = {pose_AMCL_x,pose_AMCL_y, pose_AMCL_z,  pose_AMCL_or_x,
 pose_AMCL_or_y,pose_AMCL_or_z, pose_AMCL_or_w
 }; 

 POSE pose_goal = {pose_goal_x,pose_goal_y,pose_goal_z,pose_goal_or_x,
 pose_goal_or_y,pose_goal_or_z,pose_goal_or_w   
 }   */


   
 // setGoal(pose1);
  while(ros::ok())
  {
      if(Callback_flag == true)
      {
        Callback_flag = false;
 
        if(msg_str == SETAMCL)
        {
            msg_str = "";
            setHome(pub_initialpose,amcl_point);
            ROS_INFO_STREAM("SetAmcl succeed ");
              std_msgs::String msg,msg2;  
    std::stringstream ss;  
    std::stringstream ss2;  
    ss << "设置AMCL成功";  
    ss2 << "到达目的地,导航结束";  
    msg.data = ss.str(); 
    msg2.data = ss2.str(); 
    ROS_INFO("%s", msg.data.c_str());  
    xfer_pub.publish(msg);  
     ros::spinOnce();
            
        }
        else  if(msg_str == GODRAMING)
        {
            msg_str = "";

        }
        else
        {
 
        }
      }
 
      ros::spinOnce();
     // rate_loop.sleep();
  }
   
 
  return 0; 
}  