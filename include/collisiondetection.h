#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include <nav_msgs/OccupancyGrid.h>
#include <costmap_2d/costmap_2d.h>
#include <geometry_msgs/Point.h>
#include "constants.h"
#include "lookup.h"
#include "node2d.h"
#include "pose2d.h"
#include "world_model.h"
#include "costmap_model.h"


// namespace {
// void getConfiguration(const global_planner::Node2D* node, float& x, float& y, float& t) {
//   x = node->getX();
//   y = node->getY();
//   // avoid 2D collision checking
//   t = 99;
// }

// void getConfiguration(const global_planner::Pose2D* node, float& x, float& y, float& t) {
//   x = node->getX();
//   y = node->getY();
//   t = node->getT();
// }
// }
/*!
   \brief The CollisionDetection class determines whether a given configuration q of the robot will result in a collision with the environment.

   It is supposed to return a boolean value that returns true for collisions and false in the case of a safe node.
*/

class CollisionDetection {
  public:
  //YT 这个原来的栅格地图的匹配肯定不能用，所以先删掉，实现两个部分：
  //YT 简单的碰撞检测就是在configuration space 对机器人几何中心所在网格进行查表
  //YT 复杂的碰撞检测先读取制作，机器人footprint，在costmap_2d框架下进行碰撞检测

  CollisionDetection(costmap_2d::Costmap2D* costmap, unsigned int cell_divider_, std::vector<geometry_msgs::Point> footprint_spec, 
  double origin_position_x, double origin_position_y, double gridmap_resolution)
  {
    origin_position_x_ = origin_position_x;
    origin_position_y_ = origin_position_y;
    gridmap_resolution_ = gridmap_resolution;

    costmap_ = costmap;
    this->grid = nullptr;
    HybridAStar::Lookup::collisionLookup(collisionLookup);
    
    footprint_spec_ = footprint_spec;

std::cout << "YT: printout footprint_spec_:" << std::endl;
for (unsigned int i = 0; i < footprint_spec_.size(); i++)
{
  
  std::cout << "the " << i << "node: " << footprint_spec_.at(i) << std::endl;
}


    world_model_ = new global_planner::CostmapModel(*costmap);

  }


  bool isTraversable(const global_planner::Node2D* node)
  {
    // std::cout << "YT: check the size of footprint_spec_: " << footprint_spec_.size() << std::endl;
    float x = node->getX() * gridmap_resolution_ + origin_position_x_;
    float y = node->getY() * gridmap_resolution_ + origin_position_y_;
    float theta = 0;
    /////////////////////////////////////////



    /////////////////////////////////////////
    double cost = footprintCost(x, y, theta);//YT 为正说明无碰撞，返回的是代价值，负数说明有碰撞
    std::cout << "YT: print the footprintcost of Node2D: " << cost << " （" << x << "," << y << ")" << std::endl;
    return (cost >= 0) ? true : false;


    // return true;
  }

  bool isTraversable(const global_planner::Pose2D* pose)
  {
    float x = pose->getX() * gridmap_resolution_ + origin_position_x_;
    float y = pose->getY() * gridmap_resolution_ + origin_position_y_;
    float theta = 0;

    // float x;
    // float y;
    // float t;
    // getConfiguration(pose, x, y, t);

    // return configurationTest(x, y, t);

    double cost = footprintCost(x, y, theta);
    std::cout << "YT: print the footprintcost of Pose2D: " << cost << " （" << x << "," << y << ", " << theta << ")" << std::endl;
    return (cost >= 0) ? true : false;

    // return true;
  }

  /*!
     \brief Calculates the cost of the robot taking a specific configuration q int the World W
     \param x the x position
     \param y the y position
     \param t the theta angle
     \return the cost of the configuration q of W(q)
     \todo needs to be implemented correctly
  */
  float configurationCost(float x, float y, float t) {return 0;}

  /*!
     \brief Tests whether the configuration q of the robot is in C_free
     \param x the x position
     \param y the y position
     \param t the theta angle
     \return true if it is in C_free, else false
     \YT 检测是否碰撞障碍物的关键函数
  */
  // bool configurationTest(float x, float y, float t);

  /*!
     \brief updates the grid with the world map
  */

      /**
       * @brief  Subclass will implement this method to check a footprint at a given position and orientation for legality in the world
       * @param  position The position of the robot in world coordinates
       * @param  footprint The specification of the footprint of the robot in world coordinates
       * @param  inscribed_radius The radius of the inscribed circle of the robot
       * @param  circumscribed_radius The radius of the circumscribed circle of the robot
       * @return Positive if all the points lie outside the footprint, negative otherwise
       */  
//YT 查找机器人可达性的接口函数，footprint直接存在collisiondetection里面，不需要当成参数传进去
  double footprintCost(double x_i, double y_i, double theta_i);




  void updateGrid(costmap_2d::Costmap2D* costmap)
  {
    costmap_ = costmap;

    // grid = new nav_msgs::OccupancyGrid();
    grid->info.width = costmap->getSizeInCellsX();
    grid->info.height = costmap->getSizeInCellsY();
    grid->info.resolution = costmap->getResolution();
    grid->info.origin.position.x = costmap->getOriginX();
    grid->info.origin.position.y = costmap->getOriginY();
    grid->info.origin.position.z = 0;
    grid->info.origin.orientation.x = 0;
    grid->info.origin.orientation.y = 0;
    grid->info.origin.orientation.z = 0;
    grid->info.origin.orientation.w = 1;

    grid->data.resize(costmap->getSizeInCellsX() * costmap->getSizeInCellsY());

    memcpy(grid->data.data(), costmap->getCharMap(), costmap->getSizeInCellsX() * costmap->getSizeInCellsY()*sizeof(char));


  }
 private:
  /// The occupancy grid
  nav_msgs::OccupancyGrid::Ptr grid;
  /// The collision lookup table
  global_planner::Constants::config collisionLookup[global_planner::Constants::headings * global_planner::Constants::positions];

  costmap_2d::Costmap2D* costmap_;

  global_planner::WorldModel* world_model_; ///< @brief The world model that the controller will use

  std::vector<geometry_msgs::Point> footprint_spec_;

  double origin_position_x_;
  double origin_position_y_;
  double gridmap_resolution_;

};

#endif // COLLISIONDETECTION_H