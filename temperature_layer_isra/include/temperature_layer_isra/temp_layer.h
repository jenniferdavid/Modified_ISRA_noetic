#ifndef TEMP_LAYER_H_
#define TEMP_LAYER_H_
#include <ros/ros.h>
#include <costmap_2d/layer.h>
#include <costmap_2d/layered_costmap.h>
#include <costmap_2d/costmap_layer.h>
//#include <costmap_2d/GenericPluginConfig.h>
#include <temperature_layer_isra/TemperatureLayerConfig.h>
#include <dynamic_reconfigure/server.h>

//#include <message_filters/subscriber.h>
#include <sensor_msgs/Image.h>

//#include <tf2_ros/buffer.h>
//#include <ros/time.h>

namespace temperature_layer_isra_namespace
{

class TempLayer : public costmap_2d::CostmapLayer //costmap_2d::Layer, public costmap_2d::Costmap2D
{
public:
  TempLayer();

  virtual void onInitialize();
  virtual void updateBounds(double robot_x, double robot_y, double robot_yaw, double* min_x, double* min_y, double* max_x,
                             double* max_y);
  virtual void updateCosts(costmap_2d::Costmap2D& master_grid, int min_i, int min_j, int max_i, int max_j);
  bool isDiscretized()
  {
    return true;
  }

  virtual void matchSize();


private:
  //void reconfigureCB(costmap_2d::GenericPluginConfig &config, uint32_t level);
  // dynamic_reconfigure::Server<costmap_2d::GenericPluginConfig> *dsrv_;
  void reconfigureCB(temperature_layer_isra::TemperatureLayerConfig &config, uint32_t level);
  dynamic_reconfigure::Server<temperature_layer_isra::TemperatureLayerConfig> *dsrv_;

  void temperatureCB(const sensor_msgs::Image& temp_msg); // Callback for incoming temperature messages, uses thje rad_msg type
  void updateObservations(std::list<std::pair<unsigned int, float> > &updates); // Process recent observations and return value to be added to costmap
  float* averages_;
  unsigned int* n_obs_;
  float* weight_obs_;
  unsigned int averages_size_;

  std::string node_name_; // potato
  bool update_full_layer_;
  bool rolling_window_;
  bool use_lethal_; //potato

  void getCache(std::list<std::pair<std::pair<double,double>, std::pair<float,float> > > &observation_cache);

  double upper_threshold_, lower_threshold_, ut_, lt_;
  //int upper_threshold_scale_, lower_threshold_scale_;
  int upper_threshold_scale_, lower_threshold_scale_, max_cost_; //potato
  unsigned int scaledValue(float value);

  double averaging_scale_length_, minimum_weight_;
  bool inflate_temperature_;
  int combination_method_;
  std::vector<geometry_msgs::Point> sensor_footprint_;

  ros::Subscriber temperature_sub_;
  std::list<sensor_msgs::Image> temperature_msg_buffer_;
  std::string global_frame_; // Global frame of costmap

  boost::recursive_mutex lock_; // Stop observation buffer being modified by multiple functions at once
  boost::recursive_mutex cache_;  // Stop cache from being modified when performing resizing

  void updatePreserveNoInfo(costmap_2d::Costmap2D& master_grid, int min_i, int min_j, int max_i, int max_j);
  std::vector<geometry_msgs::Point> makeSensorFootprintFromParams(ros::NodeHandle& nh);

};
}
#endif
