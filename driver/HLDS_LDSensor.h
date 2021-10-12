// -*- C++ -*-
/*!
 * @file HLDS_LDSensor.h
 * @brief LDS (Laser Distance Sensor) driver class
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2021, Noriaki Ando http://github.com/n-ando
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <boost/asio.hpp>
#include <string>
#include <vector>


namespace HLDS
{

struct LaserScan
{
	float angle_min;
	float angle_max;
	float angle_increment;
	float time_increment;
	float scan_time;
	float range_min;
	float range_max;
	std::vector<float> ranges;
	std::vector<float> intensities;
};

class LDSensor
{
public:
	/**
	* @brief Constructs a new LFCDLaser attached to the given serial port
	* @param port The string for the serial port device to attempt to connect to, e.g. "/dev/ttyUSB0"
	* @param baud_rate The baud rate to open the serial port at.
	* @param io Boost ASIO IO Service to use when creating the serial port object
	*/
	LDSensor(const std::string& port, uint32_t baud_rate);

	/**
	* @brief Default destructor
	*/
	virtual ~LDSensor();

	/**
	 * @brief Starting the sensor's motor
	 * This function causes the motor to start spinning.
	 */
	void startMotor();
	/**
	 * @brief Stopping the sensor's motor
	 * This function stops the rotation of the motor.
	 */
	void stopMotor();

	/**
	* @brief Poll the laser to get a new scan. Blocks until a complete
	* new scan is received or close is called.
	* @param scan LaserScan message pointer to fill in with the scan.
	* The caller is responsible for filling in the ROS timestamp and
	* frame_id
	*/
	void poll(LaserScan& scan);
	uint16_t rpm() { return m_rpms; }

	/**
	* @brief Close the driver down and prevent the polling loop from advancing
	*/
	void close() { m_shuttingDown = true; }

private:
	boost::asio::io_service m_io;
	// @brief The serial port the driver is attached to
	std::string m_port; 
	// @brief The baud rate for the serial connection
	uint32_t m_baudRate;
	// @brief Flag for whether the driver is supposed to be shutting down or not
	bool m_shuttingDown;
	// @brief Actual serial port object for reading/writing to the LFCD Laser Scanner
	boost::asio::serial_port m_serial;
	// @brief current motor speed as reported by the LFCD.
	uint16_t m_motorSpeed;
	// @brief RPMS derived from the rpm bytes in an LFCD packet
	uint16_t m_rpms;
};
}
