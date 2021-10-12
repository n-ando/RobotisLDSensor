// -*- C++ -*-
/*!
 * @file HLDS_LDSensor.cpp
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
#include <HLDS_LDSensor.h>
#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include <math.h>


namespace HLDS
{

// 
const uint16_t BufferLength = 2520;


LDSensor::LDSensor(const std::string& port, uint32_t baud_rate)
  : m_port(port), m_baudRate(baud_rate),
    m_shuttingDown(false), m_serial(m_io, m_port),
    m_motorSpeed(0), m_rpms(0)
{
std::cout << "ctor" << std::endl;
    m_serial.set_option(boost::asio::serial_port_base::baud_rate(m_baudRate));
std::cout << "1" << std::endl;
    startMotor();
std::cout << "2" << std::endl;
}

LDSensor::~LDSensor()
{
    stopMotor();
}

void LDSensor::startMotor()
{
std::cout << "startMotor()" << std::endl;
boost::asio::write(m_serial, boost::asio::buffer("b", 1));
std::cout << "done" << std::endl;
}

void LDSensor::stopMotor()
{
    boost::asio::write(m_serial, boost::asio::buffer("e", 1));
}

void LDSensor::poll(LaserScan& scan)
{
    uint8_t preamble = 0;
    bool got_scan = false;
    std::array<uint8_t, BufferLength> raw_bytes;
    uint8_t good_sets = 0;

    scan.angle_increment = (2.0 * M_PI / 360.0);
    scan.angle_min = 0.0;
    scan.angle_max = 2.0 * M_PI - scan.angle_increment;
    scan.range_min = 0.12;
    scan.range_max = 3.5;
    scan.ranges.resize(360);
    scan.intensities.resize(360);

    while (!m_shuttingDown && !got_scan)
    {
        // Wait until first data sync of frame: 0xFA, 0xA0
        boost::asio::read(m_serial,
                            boost::asio::buffer(&raw_bytes[preamble],1));

        // detect the first byte of start frame
        if (preamble == 0 && raw_bytes[preamble] == 0xFA)
        {
            preamble = 1;
            continue;
        }

        // detect the start frame (0xFA 0xA0), and reading the message body
        if (preamble == 1 && raw_bytes[preamble] == 0xA0)
        {
            preamble = 0;
            got_scan = true;
            boost::asio::read(m_serial,
                              boost::asio::buffer(&raw_bytes[2],
                              BufferLength - 2));

            // read data in sets of 60
            for (uint16_t i = 0; i < raw_bytes.size(); i += 42)
            {   // checking CRC [0xFA, 0xA0+"#/42"]
                if (raw_bytes[i] == 0xFA && 
                    raw_bytes[i + 1] == (0xA0 + i / 42))
                {
                    good_sets++;
                    m_motorSpeed += (raw_bytes[i + 3] << 8) + raw_bytes[i + 2];

                    for (uint16_t j = i + 4; j < i + 40; j = j + 6)
                    {
                        int index = 6 * (i / 42) + (j - 4 - i) / 6;
                        // intensity
                        uint8_t byte0 = raw_bytes[j];   // intensity [1]
                        uint8_t byte1 = raw_bytes[j+1]; // intensity [2]
                        uint16_t intensity = (byte1 << 8) + byte0;
                        scan.intensities[359 - index] = intensity;
                        // range
                        uint8_t byte2 = raw_bytes[j+2]; // range in m [1]
                        uint8_t byte3 = raw_bytes[j+3]; // range in m [2]
                        uint16_t range = (byte3 << 8) + byte2;    
                        scan.ranges[359 - index] = range / 1000.0;
                    }
                }
            }
            m_rpms = m_motorSpeed / good_sets / 10;
            scan.time_increment = (float)(1.0 / (m_rpms * 6));
            scan.scan_time = scan.time_increment * 360;
            continue;
        }
        preamble = 0;
    }
}

}

// g++ -I. -lboost_system -lpthread -o HLDS_LDS HLDS_LDS.cpp
#if 1 
int main(int argc, char **argv)
{
    HLDS::LDSensor ldsensor("/dev/ttyUSB0", 230400);
    HLDS::LaserScan scan;

    while (1)
    {
        ldsensor.poll(scan);
        std::cout << "angle_min: " << scan.angle_min << std::endl;
        std::cout << "angle_max: " << scan.angle_max << std::endl;
        std::cout << "angle_inc: " << scan.angle_increment << std::endl;
        std::cout << "scan_time: " << scan.scan_time << std::endl;
        std::cout << "range_min: " << scan.range_min << std::endl;
        std::cout << "range_max: " << scan.range_max << std::endl;
        for (size_t i(0); i < scan.ranges.size(); ++i)
        {
            std::cout << scan.ranges[i] << " ";
        }
        std::cout << std::endl;
        
    }
    ldsensor.close();
    return 0;
}
#endif
