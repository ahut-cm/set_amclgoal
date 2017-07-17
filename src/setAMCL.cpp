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
bool SetAmcl_flag = false;
string msg_str = "";
POSE set_amcl_java;
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

void poseCallback(const std_msgs::String::ConstPtr &msg)
{
     ROS_INFO_STREAM("Topic is Subscriber ");
     std::cout<<"get topic text: " << msg->data << std::endl;
      
     Callback_flag = true;
     msg_str = msg->data;
}  

void amclCallback(const set_amclgoal::amcl_type::ConstPtr &msg)
{
    //set_amcl_java.X = msg.A;
    set_amcl_java.X = msg->X;
    set_amcl_java.Y = msg->Y;
    set_amcl_java.Z = msg->Z;
    set_amcl_java.or_x = msg->OR_X;
    set_amcl_java.or_y = msg->OR_Y;
    set_amcl_java.or_z = msg->OR_Z;
    set_amcl_java.or_w = msg->OR_W;
    SetAmcl_flag = true;
}  


int main(int argc, char** argv){  
    ros::init(argc, argv, "base_amcl_set"); 
    ros::NodeHandle nh;
    ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 10);
    ros::Subscriber sub = nh.subscribe("/base/amcl_set",10,poseCallback);
    ros::Subscriber sub2 = nh.subscribe("/base/amcl_set_point",10,amclCallback);
    ros::Publisher xfer_pub = nh.advertise<std_msgs::String>("xfwords", 10);
      //tell the action client that we want to spin a thread by default  
      MoveBaseClient ac("move_base", true);  
      
      //wait for the action server to come up  
  /*    while(!ac.waitForServer(ros::Duration(5.0))){  
        ROS_INFO("Waiting for the move_base action server to come up");
      }    */
     // POSE pose_AMCL = {0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0};

     POSE amcl_point_launch;
     ros::param::get("~AMCL_Point_x",amcl_point_launch.X);
     ros::param::get("~AMCL_Point_y",amcl_point_launch.Y);
     ros::param::get("~AMCL_Point_z",amcl_point_launch.Z);
     ros::param::get("~AMCL_orientation_x",amcl_point_launch.or_x);
     ros::param::get("~AMCL_orientation_y",amcl_point_launch.or_y);
     ros::param::get("~AMCL_orientation_z",amcl_point_launch.or_z);
     ros::param::get("~AMCL_orientation_w",amcl_point_launch.or_w);
     

 while(ros::ok())
  {
      if(Callback_flag == true)
      {
        Callback_flag = false;
 
        if(msg_str == SETAMCL)
        {
            msg_str = "";
            setHome(pub_initialpose,set_amcl_java);
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

        else
        {
 
        }
      }

       if(SetAmcl_flag == true)
      {
        SetAmcl_flag = false;
 
            setHome(pub_initialpose,set_amcl_java);
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

        else
        {
 
        }
      
 
      ros::spinOnce();
     // rate_loop.sleep();
  }
   
 
  return 0; 
}  