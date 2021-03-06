#include<pedstrain_layers/pedstrain_layer.h>
#include <pluginlib/class_list_macros.h>
#include <std_msgs/Float32.h>
 
PLUGINLIB_EXPORT_CLASS(pedstrain_layer_namespace::PedstrainLayer, costmap_2d::Layer)
 
using costmap_2d::LETHAL_OBSTACLE;
 
namespace pedstrain_layer_namespace
{
 
  PedstrainLayer::PedstrainLayer() {}

  void PedstrainLayer::onInitialize()
  {
    ros::NodeHandle nh("~/" + name_);
    current_ = true;

    //subscribe function need to put in initial function
    ros::NodeHandle ped_nh("pedestrain");
    pedestrain_sub_ = ped_nh.subscribe<std_msgs::Float32>("pedestrain_point", 1, boost::bind(&PedstrainLayer::pedestrainCB, this, _1));
  
    dsrv_ = new dynamic_reconfigure::Server<costmap_2d::GenericPluginConfig>(nh);
    dynamic_reconfigure::Server<costmap_2d::GenericPluginConfig>::CallbackType cb = boost::bind(
        &PedstrainLayer::reconfigureCB, this, _1, _2);
    dsrv_->setCallback(cb);

  }
  //add callback function detail
  void PedstrainLayer::pedestrainCB(const std_msgs::Float32::ConstPtr& msg)
  {
    point1 = msg->data;
    ROS_INFO("I get [%f]", point1);
  }

  void PedstrainLayer::reconfigureCB(costmap_2d::GenericPluginConfig &config, uint32_t level)
  {
    enabled_ = config.enabled;
  }
 
  void PedstrainLayer::updateBounds(double origin_x, double origin_y, double origin_yaw, double* min_x,
                                            double* min_y, double* max_x, double* max_y)
  {
    if (!enabled_)
      return;
    
    // ROS_INFO("I heard: [%f]", point1);
    mark_x_ = origin_x + cos(origin_yaw);
    mark_y_ = origin_y + sin(origin_yaw);
    // mark_x_ = origin_x + 7;
    // mark_y_ = origin_y + 7;
    *min_x = std::min(*min_x, mark_x_);
    *min_y = std::min(*min_y, mark_y_);
    *max_x = std::max(*max_x, mark_x_);
    *max_y = std::max(*max_y, mark_y_);
  }
 
  void PedstrainLayer::updateCosts(costmap_2d::Costmap2D& master_grid, int min_i, int min_j, int max_i,
                                            int max_j)
  {
    if (!enabled_)
      return;
    unsigned int mx;
    unsigned int my;
    if(master_grid.worldToMap(mark_x_, mark_y_, mx, my)){
      master_grid.setCost(mx, my, LETHAL_OBSTACLE);
    }
  }
 
} // end namespace

