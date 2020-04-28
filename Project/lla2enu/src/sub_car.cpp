#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/NavSatFix.h"
#include <tf/transform_broadcaster.h>
#include "nav_msgs/Odometry.h"
#include <math.h>  
#include <dynamic_reconfigure/server.h> //standard libraries for dynamic reconfiguration

class converter
{
	private:
		float long_in;
		float lat_in;
		float alt_in;
		ros::NodeHandle n;
		ros::Subscriber sub;
		ros::Publisher pub;
		ros::Publisher pub2;
		tf::TransformBroadcaster br; //broadcaster èer le tf
	public:
		converter()
		{ 
			n.getParam("/long_in",long_in);
			n.getParam("/lat_in",lat_in);	
			n.getParam("/alt_in",alt_in);	
			//ROS_INFO("initial position: [%f,%f, %f]", long_in, lat_in,alt_in);

			sub = n.subscribe("/swiftnav/front/gps_pose", 1000, &converter::chatterCallback, this);
			pub = n.advertise<nav_msgs::Odometry>("/car/Odom",1);
		}
		
	void chatterCallback(const sensor_msgs::NavSatFix::ConstPtr& msg)
	{
		// ROS_INFO("Input position: [%f,%f, %f]", msg->latitude, msg->longitude,msg->altitude);

		// fixed values 
		// TODO: trasformare in funzione

		double a = 6378137;
		double b = 6356752.3142;
		double f = (a - b) / a;
		double e_sq = f * (2-f);
		float deg_to_rad = 0.0174533;

		// input data from msg
		float latitude = msg->latitude;
		float longitude = msg->longitude;
		float h = msg->altitude;
        
 

		// fixed position [TODO: ricever come argomento]
		float latitude_init = lat_in;
		float longitude_init = long_in;
		float h0 = alt_in;

		// lla to ecef
		float lamb = deg_to_rad*(latitude);
		float phi = deg_to_rad*(longitude);
		float s = sin(lamb);
		float N = a / sqrt(1 - e_sq * s * s);

		float sin_lambda = sin(lamb);
		float cos_lambda = cos(lamb);
		float sin_phi = sin(phi);
		float cos_phi = cos(phi);
		  
		float x = (h + N) * cos_lambda * cos_phi;
		float y = (h + N) * cos_lambda * sin_phi;
		float z = (h + (1 - e_sq) * N) * sin_lambda;

		// ROS_INFO("ECEF position: [%f,%f, %f]", x, y,z);
  

		// ecef to enu
		 
		lamb = deg_to_rad*(latitude_init);
		phi = deg_to_rad*(longitude_init);
		s = sin(lamb);
		N = a / sqrt(1 - e_sq * s * s);

		sin_lambda = sin(lamb);
		cos_lambda = cos(lamb);
		sin_phi = sin(phi);
		cos_phi = cos(phi);

		float x0 = (h0 + N) * cos_lambda * cos_phi;
		float y0 = (h0 + N) * cos_lambda * sin_phi;
		float z0 = (h0 + (1 - e_sq) * N) * sin_lambda;

		float xd = x - x0;
		float yd = y - y0;
		float zd = z - z0;

		float xEast = -sin_phi * xd + cos_phi * yd;
		float yNorth = -cos_phi * sin_lambda * xd - sin_lambda * sin_phi * yd + cos_lambda * zd;
		float zUp = cos_lambda * cos_phi * xd + cos_lambda * sin_phi * yd + sin_lambda * zd;

		// ROS_INFO("ENU position: [%f,%f, %f]", xEast, yNorth,zUp);
		// preparazione messaggio di risposta sotto forma di Odom
		nav_msgs::Odometry messaggio;
		messaggio.header.frame_id="world";
		messaggio.header.stamp=ros::Time::now();
		messaggio.pose.pose.position.x=xEast;
		messaggio.pose.pose.position.y=yNorth;
		messaggio.pose.pose.position.z=zUp;
		messaggio.pose.pose.orientation.x=0;
		messaggio.pose.pose.orientation.y=0;
		messaggio.pose.pose.orientation.z=0;
		messaggio.pose.pose.orientation.w=1;
		// pubblichiamolo mo
		pub.publish(messaggio);
        
        //Creazione messaggio di errore nel campo twist.twist.linear
        if (latitude == 0 && longitude == 0 && h==0) 
        { 
            messaggio.twist.twist.linear.x = -1;
        } 
        else  
        { 
            messaggio.twist.twist.linear.x = 0;
        } 

        // pubblichiamolo 
		pub.publish(messaggio);
        
        
		// Pubblicazione risposte sotto forma di TF
		tf::Transform transform;
		transform.setOrigin( tf::Vector3(xEast, yNorth, zUp) );
		tf::Quaternion q;
		q.setRPY(0, 0, 0); //set roll and pitch to 0 as we are in 2d
		transform.setRotation(q);
		br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", "car"));
	}
};

int main(int argc, char **argv)
{
	ros::init(argc, argv, "car");
	converter MyConverter;	
  	ros::spin();

	return 0;
}


