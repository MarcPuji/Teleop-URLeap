#include "ros/ros.h"
#include <geometry_msgs/Pose.h>
#include <stdio.h>

#include <geometry_msgs/TransformStamped.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/transform_broadcaster.h>

#include <visualization_msgs/MarkerArray.h>

#include "ur5ik/UR5IK.h"

// Callback Function
float leap_x, leap_y;
void Leap_CB(const visualization_msgs::MarkerArray msg){
	//leap_x = msg.markers[0].pose.position.x;
    //leap_y = msg.markers[0].pose.position.y;
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "tfleap_publisher");
    ros::NodeHandle n;

    // Service Client Initializer
    ros::ServiceClient IKClient = n.serviceClient<ur5ik::UR5IK>("UR5IK");
    // Subscriber to TargetPose
    ros::Subscriber LeapSubscriber = n.subscribe<visualization_msgs::MarkerArray>("leap_motion/visualization_marker_array", 1000, Leap_CB);

    //Create the request and response objects.
    ur5ik::UR5IK::Request req;
    ur5ik::UR5IK::Response resp;

    // Create a broadcaster
    static tf2_ros::TransformBroadcaster br;
    // Create the Stapmed Transform for the Leap Motion
    geometry_msgs::TransformStamped transformStamped;

    ros::Rate loop_rate(60);
    ros::spinOnce();

    while(ros::ok()){
    	// The initial position of ee_link is (0.69, 0.19, 0.295) so let's move from there
    	// Apply the necessary modifications given the Leap Motion situation
    	transformStamped.header.stamp = ros::Time::now();
    	transformStamped.header.frame_id = "world";
    	transformStamped.child_frame_id = "leap_ee";    
    	transformStamped.transform.translation.x = 0.69; //+ leap_x;
    	transformStamped.transform.translation.y = 0.19; //+ leap_y;
    	transformStamped.transform.translation.z = 0.295;
    	// The EndEffector needs to be pointing down all the time
    	tf2::Quaternion q;
    	q.setRPY(-3.1416, 0, 0);
    	transformStamped.transform.rotation.x = q.x();
    	transformStamped.transform.rotation.y = q.y();
    	transformStamped.transform.rotation.z = q.z();
    	transformStamped.transform.rotation.w = q.w();
    	// Broadcast the Transform
    	br.sendTransform(transformStamped);

    	// Build the request
    	req.pose.position.x = transformStamped.transform.translation.x;
    	req.pose.position.y = transformStamped.transform.translation.y;
    	req.pose.position.z = transformStamped.transform.translation.z;
    	req.pose.orientation = transformStamped.transform.rotation;

    	// Execute the command if we press the "a" button with keyboard
    	//int c = getchar();
    	//if(c == 'a'){
   		//	ros::service::waitForService("UR5IK", ros::Duration(5));
    	//	IKClient.call(req,resp);
    	//}
    	ros::spinOnce();
    }
    return 0;
}
