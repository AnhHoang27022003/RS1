#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/header.hpp"
#include <chrono>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.hpp>
#include <opencv2/opencv.hpp>

using namespace std::chrono_literals;

class ImageModifierNode : public rclcpp::Node {
public:
  ImageModifierNode() : Node("image_modifier_node") {
    // Initialize subscriber and publisher
    subscriber_ = this->create_subscription<sensor_msgs::msg::Image>(
        "camera/image_raw", 10,
        std::bind(&ImageModifierNode::image_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<sensor_msgs::msg::Image>("camera_modified", 10);
  }

private:
  void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
    // Convert ROS image message to OpenCV image
    cv_bridge::CvImagePtr cv_ptr;
    try {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    } catch (cv_bridge::Exception& e) {
      RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
      return;
    }

    // Get image dimensions
    int width = cv_ptr->image.cols;
    int height = cv_ptr->image.rows;
    int center_x = width / 2;
    int center_y = height / 2;
    int radius = 200; // Set radius to 10% of the smallest dimension

    // Draw a hollow circle in the middle of the image
    cv::circle(cv_ptr->image, cv::Point(center_x, center_y), radius, CV_RGB(255, 0, 0), 2);

    // Convert OpenCV image back to ROS image message and publish
    sensor_msgs::msg::Image::SharedPtr modified_msg =
        cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", cv_ptr->image).toImageMsg();
    publisher_->publish(*modified_msg);
  }

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscriber_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ImageModifierNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
