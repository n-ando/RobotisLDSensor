// -*- C++ -*-
/*!
 * @file  RobotisLDSensorTest.cpp
 * @brief Robotis LDS-01 RTC
 * @date $Date$
 *
 * $Id$
 */

#include "RobotisLDSensorTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* robotisldsensor_spec[] =
  {
    "implementation_id", "RobotisLDSensorTest",
    "type_name",         "RobotisLDSensorTest",
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
RobotisLDSensorTest::RobotisLDSensorTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_rangeOut("range", m_range)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
RobotisLDSensorTest::~RobotisLDSensorTest()
{
}



RTC::ReturnCode_t RobotisLDSensorTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("range", m_rangeIn);

  // Set OutPort buffer

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
RTC::ReturnCode_t RobotisLDSensorTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RobotisLDSensorTest::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RobotisLDSensorTest::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t RobotisLDSensorTest::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t RobotisLDSensorTest::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t RobotisLDSensorTest::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RobotisLDSensorTest::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RobotisLDSensorTest::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t RobotisLDSensorTest::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RobotisLDSensorTest::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RobotisLDSensorTest::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{

  void RobotisLDSensorTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(robotisldsensor_spec);
    manager->registerFactory(profile,
                             RTC::Create<RobotisLDSensorTest>,
                             RTC::Delete<RobotisLDSensorTest>);
  }

};


