#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "std_msgs/msg/header.hpp"
#include <chrono>

using namespace std::chrono_literals;

class LaserScanListenerRepublisher : public rclcpp::Node {
public:
  LaserScanListenerRepublisher() : Node("laserscan_listener_republisher") {
    // Initialize subscriber and publisher
    subscriber_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "scan", 10,
        std::bind(&LaserScanListenerRepublisher::laserscan_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<sensor_msgs::msg::LaserScan>("scan_modified", 10);
  }

private:
  void laserscan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
  
    sensor_msgs::msg::LaserScan modified_scan = *msg;
    modified_scan.ranges.clear();
    modified_scan.intensities.clear();
    for (size_t i = 0; i < msg->ranges.size(); i += 3) {
      modified_scan.ranges.push_back(msg->ranges[i]);
      if (!msg->intensities.empty()) {
        modified_scan.intensities.push_back(msg->intensities[i]);
      }
    }
    publisher_->publish(modified_scan);
    std::cout << "ORIGINAL SCAN: " << msg->ranges.size() << " ";
    std::cout << "FILTERED SCAN: " << modified_scan.ranges.size() << std::endl;
  }

  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscriber_;
  rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<LaserScanListenerRepublisher>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
