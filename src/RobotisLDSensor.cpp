// -*- C++ -*-
/*!
 * @file  RobotisLDSensor.cpp
 * @brief Robotis LDS-01 RTC
 * @date $Date$
 *
 * $Id$
 */

#include "RobotisLDSensor.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

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
    "conf.default.port_name", "/dev/ttyUSB0",
    "conf.default.baudrate", "230400",
    "conf.default.debug", "0",
    "conf.default.scale", "1.0",
    "conf.default.offset", "5.0",
    "conf.default.geometry_x", "0.0",
    "conf.default.geometry_y", "0.0",
    "conf.default.geometry_z", "0.0",

    // Widget
    "conf.__widget__.port_name", "text",
    "conf.__widget__.baudrate", "text",
    "conf.__widget__.debug", "radio",
    "conf.__widget__.scale", "slider.0.001",
    "conf.__widget__.offset", "slider",
    "conf.__widget__.geometry_x", "text",
    "conf.__widget__.geometry_y", "text",
    "conf.__widget__.geometry_z", "text",
    // Constraints
    "conf.__constraints__.debug", "(0, 1)",
    "conf.__constraints__.scale", "0.001<x<1000.0",
    "conf.__constraints__.offset", "-180.0<x<180.0",

    "conf.__type__.port_name", "string",
    "conf.__type__.baudrate", "int",
    "conf.__type__.debug", "int",
    "conf.__type__.scale", "float",
    "conf.__type__.offset", "double",
    "conf.__type__.geometry_x", "double",
    "conf.__type__.geometry_y", "double",
    "conf.__type__.geometry_z", "double",

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
  bindParameter("port_name", m_port_name, "/dev/ttyUSB0");
  bindParameter("baudrate", m_baudrate, "230400");
  bindParameter("debug", m_debug, "0");
  bindParameter("scale", m_scale, "1.0");
  bindParameter("offset", m_offset, "5.0");
  bindParameter("geometry_x", m_geometry_x, "0.0");
  bindParameter("geometry_y", m_geometry_y, "0.0");
  bindParameter("geometry_z", m_geometry_z, "0.0");
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
    RTC_DEBUG(("onActivated()"));
    try
    {
        m_ldsensor = new HLDS::LDSensor(m_port_name, m_baudrate);
    }
    catch (...)
    {
        RTC_DEBUG(("LDSensor device open failed"));
        RTC_DEBUG(("Port name: %s", m_port_name));
        RTC_DEBUG(("Baud rate: %d", m_baudrate));
        return RTC::RTC_ERROR; 
    }
    RTC_INFO(("LDSensor opened: %s, %d", m_port_name, m_baudrate));

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
    RTC_DEBUG(("LDSensor motor stopped."));
    m_ldsensor->close();
    RTC_DEBUG(("LDSensor device closed."));
    delete m_ldsensor;
    RTC_PARANOID(("LDSensor object deleted."));
    return RTC::RTC_OK;
}


RTC::ReturnCode_t RobotisLDSensor::onExecute(RTC::UniqueId ec_id)
{
    HLDS::LaserScan scan;
    m_ldsensor->poll(scan);
    size_t count = scan.ranges.size();
    double incr = scan.angle_increment;
    // https://emanual.robotis.com/assets/docs/LDS_Basic_Specification.pdf
    m_range.config.minAngle = scan.angle_min;
    m_range.config.maxAngle = scan.angle_max;
    // spec: angular resolution = 1 degree
    m_range.config.angularRes = incr;
    m_range.config.minRange = 120 / 1000.0;
    m_range.config.maxRange = 3500 / 1000.0;
    m_range.config.rangeRes = 15 / 1000.0; // 15mm (12mm-499mm)
    // spec: 300+-10rpm, 
    m_range.config.frequency = m_ldsensor->rpm() / 60.0; // rpm->Hz spec 1.8kHz

    if (m_debug == 1)
      {
        std::cout << "min angle: " << scan.angle_min << std::endl;
        std::cout << "min angle: " << scan.angle_min * 180 / M_PI << " [deg]" << std::endl;
        std::cout << "max angle: " << scan.angle_max << std::endl;
        std::cout << "max angle: " << scan.angle_max * 180 / M_PI << " [deg]" << std::endl;
        std::cout << "angle res: " << incr << std::endl;
        std::cout << "angle res: " << incr * 180 / M_PI << " [deg]" << std::endl;
        std::cout << "min range: " << m_range.config.minRange << " [m]"<< std::endl;
        std::cout << "max range: " << m_range.config.maxRange << " [m]" << std::endl;
        std::cout << "range res: " << m_range.config.rangeRes << " [m]" << std::endl;
        std::cout << "freq:      " << m_ldsensor->rpm() << " [rpm]" << std::endl;
        std::cout << "freq:      " << m_range.config.frequency << " [Hz]" << std::endl;
        std::cout << "range num: " << count << std::endl; 
      }

    m_range.ranges.length(count);
    for (size_t i = 0; i < scan.ranges.size(); ++i)
    {
      // angluar offset
      // offset[rad]/angular_resolution[rad/index] => index_offset
      // index_offset % range_index_size => shifted index
      int i_d;
      if (m_offset < 0)
        {
          i_d = (i - int((m_offset / 180 * M_PI) / incr)) % count;
        }
      else
        {
          i_d = (i + count - int((m_offset / 180 * M_PI) / incr)) % count;
        }
      m_range.ranges[i] = scan.ranges[i_d] * m_scale;
//      if (m_debug == 1 && i % 15 < 5)
//        {
//          if (i % 15 == 0) { std::cout << std::setw(4) << i << ": "; }
//          std::cout << std::setw(5) << scan.ranges[i_d] * m_scale;
//          std::cout << " [" << std::setw(3) << i_d << "] ";
//          if (i % 5 == 4) { std::cout << std::endl; }
//        }
      if (m_debug == 1 && i % 45 == 0)
        {
          double len = m_range.ranges[i];
          std::cout << std::setw(5) << i << ": ";
          size_t out_len = std::min(len * 100, 60.0);
          for (size_t g = 0; g < size_t(out_len); ++g)
            {
              std::cout << "|";
            }
          std::cout << " " << int(len * 100) << "[cm]" << std::endl;
        }
    }
    if (m_debug == 1) { std::cout << std::endl; }

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


