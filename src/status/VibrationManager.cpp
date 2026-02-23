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

#include "VibrationManager.h"

namespace SlimeVR {

void VibrationManager::setup() {
	m_Enabled = (m_Pin < 255);

	if (!m_Enabled) {
		m_Logger.info("Vibration motor disabled (pin not configured)");
		return;
	}

	pinMode(m_Pin, OUTPUT);
	digitalWrite(m_Pin, LOW);
	m_Logger.info("Vibration motor initialized on pin %d", m_Pin);
}

void VibrationManager::vibrate(uint16_t durationMs) {
	if (!m_Enabled) {
		return;
	}

	if (durationMs == 0) {
		// Turn off vibration
		digitalWrite(m_Pin, LOW);
		m_IsVibrating = false;
		m_VibrationDuration = 0;
		return;
	}

	// Start vibration
	digitalWrite(m_Pin, HIGH);
	m_IsVibrating = true;
	m_VibrationStartTime = millis();
	m_VibrationDuration = durationMs;
	m_Logger.debug("Vibration triggered for %d ms", durationMs);
}

void VibrationManager::update() {
	if (!m_Enabled || !m_IsVibrating) {
		return;
	}

	// Check if vibration duration has elapsed (overflow-safe)
	unsigned long elapsed = millis() - m_VibrationStartTime;
	if (elapsed >= m_VibrationDuration) {
		digitalWrite(m_Pin, LOW);
		m_IsVibrating = false;
		m_VibrationDuration = 0;
	}
}

}  // namespace SlimeVR
