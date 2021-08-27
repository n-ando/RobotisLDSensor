// -*- C++ -*-
/*!
 * @file  RobotisLDSensor.cpp
 * @brief Robotis LDS-01 RTC
 * @date $Date$
 *
 * $Id$
 */

#include "RobotisLDSensor.h"

// Module specification
// <rtc-template block="module_spec">
static const char* robotisldsensor_spec[] =
  {
    "implementation_id", "RobotisLDSensor",
    "type_name",         "RobotisLDSensor",
    "description",       "Robotis LDS-01 RTC",
    "version",           "1.0.0",
    "vendor",            "aist",
    "category",          "Sensor",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.port_name", "ttyUSB0",
    "conf.default.baudrate", "230400",
    "conf.default.debug", "0",
    "conf.default.encoding", "2",
    "conf.default.geometry_x", "0",
    "conf.default.geometry_y", "0",
    "conf.default.geometry_z", "0",
    "conf.default.scale", "1.0",

    // Widget
    "conf.__widget__.port_name", "text",
    "conf.__widget__.baudrate", "text",
    "conf.__widget__.debug", "text",
    "conf.__widget__.encoding", "spin",
    "conf.__widget__.geometry_x", "text",
    "conf.__widget__.geometry_y", "text",
    "conf.__widget__.geometry_z", "text",
    "conf.__widget__.scale", "text",
    // Constraints
    "conf.__constraints__.encoding", "{twochar:2,threechar:3}",

    "conf.__type__.port_name", "string",
    "conf.__type__.baudrate", "int",
    "conf.__type__.debug", "int",
    "conf.__type__.encoding", "int",
    "conf.__type__.geometry_x", "double",
    "conf.__type__.geometry_y", "double",
    "conf.__type__.geometry_z", "double",
    "conf.__type__.scale", "float",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
RobotisLDSensor::RobotisLDSensor(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_rangeOut("range", m_range)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
RobotisLDSensor::~RobotisLDSensor()
{
}



RTC::ReturnCode_t RobotisLDSensor::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers

  // Set OutPort buffer
  addOutPort("range", m_rangeOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("port_name", m_port_name, "ttyUSB0");
  bindParameter("baudrate", m_baudrate, "115200");
  bindParameter("debug", m_debug, "0");
  bindParameter("encoding", m_encoding, "2");
  bindParameter("geometry_x", m_geometry_x, "0");
  bindParameter("geometry_y", m_geometry_y, "0");
  bindParameter("geometry_z", m_geometry_z, "0");
  bindParameter("scale", m_scale, "1.0");
  // </rtc-template>

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RobotisLDSensor::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RobotisLDSensor::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RobotisLDSensor::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t RobotisLDSensor::onActivated(RTC::UniqueId ec_id)
{
  m_ldsensor = new HLDS::LDSensor(m_port_name, m_baudrate);

  std::cout << "[RobotisLDSensor RTC] Staring in (" << m_port_name;
  std::cout << ", baudrate=" << m_baudrate << ")" << std::endl; 

  m_range.geometry.geometry.pose.position.x = m_geometry_x;
  m_range.geometry.geometry.pose.position.y = m_geometry_y;
  m_range.geometry.geometry.pose.position.z = m_geometry_z;

  m_range.geometry.geometry.pose.orientation.r = 0.0;
  m_range.geometry.geometry.pose.orientation.p = 0.0;
  m_range.geometry.geometry.pose.orientation.y = 0.0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t RobotisLDSensor::onDeactivated(RTC::UniqueId ec_id)
{
  m_ldsensor->stopMotor();
  m_ldsensor->close();
  delete m_ldsensor;
  return RTC::RTC_OK;
}


RTC::ReturnCode_t RobotisLDSensor::onExecute(RTC::UniqueId ec_id)
{
    HLDS::LaserScan scan;
    m_ldsensor->poll(scan);
    size_t count = scan.ranges.size();
    // https://emanual.robotis.com/assets/docs/LDS_Basic_Specification.pdf
    m_range.config.minAngle = scan.angle_min;
    m_range.config.maxAngle = scan.angle_max;
    // spec: angular resolution = 1 degree
    m_range.config.angularRes = scan.angle_increment;
    m_range.config.minRange = 120 / 1000.0;
    m_range.config.maxRange = 3500 / 1000.0;
    m_range.config.rangeRes = 15 / 1000.0; // 15mm (12mm-499mm)
    // spec: 300+-10rpm, 
    m_range.config.frequency = m_ldsensor->rpm() / 60; // rpm->Hz spec 1.8kHz

    m_range.ranges.length(count);

    for (size_t i = 0; i < scan.ranges.size(); ++i)
    {
      int idx = (count - i - 1  + count / 2) % count;
      m_range.ranges[i] = scan.ranges[i] * m_scale;
    }

    m_rangeOut.write();
    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RobotisLDSensor::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RobotisLDSensor::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t RobotisLDSensor::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RobotisLDSensor::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RobotisLDSensor::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{

  void RobotisLDSensorInit(RTC::Manager* manager)
  {
    coil::Properties profile(robotisldsensor_spec);
    manager->registerFactory(profile,
                             RTC::Create<RobotisLDSensor>,
                             RTC::Delete<RobotisLDSensor>);
  }

};


