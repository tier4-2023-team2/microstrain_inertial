/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Parker-Lord Driver Definition File
//
// Copyright (c) 2017, Brian Bingham
// Copyright (c) 2020, Parker Hannifin Corp
//
// This code is licensed under MIT license (see LICENSE file for details)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MICROSTRAIN_INERTIAL_DRIVER_COMMON_UTILS_MIP_ROS_CONNECTION_H
#define MICROSTRAIN_INERTIAL_DRIVER_COMMON_UTILS_MIP_ROS_CONNECTION_H

#include <vector>
#include <string>
#include <memory>
#include <fstream>

#include "mip/mip_device.hpp"

#include "microstrain_inertial_driver_common/utils/ros_compat.h"

namespace microstrain
{

// Predeclare here so we can pass as a parameter to the later configure step
class RosMipDevice;

/**
 * ROS implementation of the MIP connection class
 */
class RosConnection : public mip::Connection
{
 public:
  /**
   * \brief Constructs the ROS connection given a reference to the node that initialized it.
   * \param node Reference to the node that is initializing the connection
   */
  explicit RosConnection(RosNodeType* node);

  /**
   * \brief Connects to the MIP device. When this function is finished, if it returns true, the device will be open, but not necesarilly ready to communicate
   * \param config_node Reference to a ROS node object that contains configuration information
   * \param port The serial port to connect to
   * \param baudrate The baudrate to open the serial port at
   * \return true if the connection was successful and false otherwise
   */
  bool connect(RosNodeType* config_node, const std::string& port, const int32_t baudrate);

  /**
   * \brief Configures the RosConnection object. This should be called after connect
   * \param config_node Reference to a ROS node object that contains configuration information
   * \param device The device to use to read information from the device
   * \return true if the configuration was successful and false otherwise
   */
  bool configure(RosNodeType* config_node, RosMipDevice* device);

  /**
   * \brief Returns the parse timeout for this connection object
   * \return the parse timeout to use with this connection object
   */
  mip::Timeout parseTimeout() const;

  /**
   * \brief Returns the base reply timeout for this connection object
   * \return the base reply timeout to use with this connection object
   */
  mip::Timeout baseReplyTimeout() const;

  /**
   * \brief Returns the NMEA messages collected by the connection, and clears the list of messages on the connection object
   * \return List of NMEA messages collected by the connection
   */
  std::vector<NMEASentenceMsg> nmeaMsgs();

  // Implemented in order to satisfy the requirements for the MIP connection
  bool sendToDevice(const uint8_t* data, size_t length) final;
  bool recvFromDevice(uint8_t* buffer, size_t max_length, mip::Timeout timeout, size_t* count_out, mip::Timestamp* timestamp_out) final;

 private:
  /**
   * \brief Extracts NMEA data from a byte array
   * \param data  Raw bytes that may contain a NMEA sentence
   * \param data_len  Length of the data array in bytes
   */
  void extractNmea(const uint8_t* data, size_t data_len);

  RosNodeType* node_;  /// Reference to the ROS node that created this connection

  std::unique_ptr<mip::Connection> connection_;  /// Connection object used to actually interact with the device
  mip::Timeout parse_timeout_;  /// Parse timeout given the type of connection configured
  mip::Timeout base_reply_timeout_;  /// Base reply timeout given the type of connection configured

  bool should_record_;  /// Whether or not we should record binary data on this connection
  std::ofstream record_file_;  /// The file that the binary data should be recorded to

  bool should_extract_nmea_;  /// Whether or not we should attempt to parse and extract NMEA sentences on this connection
  std::string nmea_string_;  /// Cached data read from the port, used to extraxt NMEA messages
  std::vector<NMEASentenceMsg> nmea_msgs_;  /// List of NMEA messages received by this connection
};

}  // namespace microstrain

#endif  // MICROSTRAIN_INERTIAL_DRIVER_COMMON_UTILS_MIP_ROS_CONNECTION_H
