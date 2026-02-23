/*
	SlimeVR Code is placed under the MIT license
	Copyright (c) 2024 SlimeVR Contributors

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/
#ifndef SLIMEVR_VIBRATIONMANAGER_H
#define SLIMEVR_VIBRATIONMANAGER_H

#include <Arduino.h>

#include "../globals.h"
#include "../logging/Logger.h"

namespace SlimeVR {

class VibrationManager {
public:
	void setup();

	/*!
	 *  @brief Triggers the vibration motor for a specified duration
	 *  @param durationMs Duration in milliseconds (0-65535)
	 */
	void vibrate(uint16_t durationMs);

	/*!
	 *  @brief Updates vibration state (non-blocking)
	 *  Should be called in main loop
	 */
	void update();

private:
	uint8_t m_Pin = VIBRATION_PIN;
	bool m_Enabled = m_Pin >= 0 && m_Pin < 255;
	unsigned long m_VibrationEndTime = 0;
	bool m_IsVibrating = false;

	Logging::Logger m_Logger = Logging::Logger("VibrationManager");
};

}  // namespace SlimeVR

#endif
