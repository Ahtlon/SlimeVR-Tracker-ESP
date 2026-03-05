# Vibration Motor Protocol

This document describes the communication protocol for triggering a vibration motor on SlimeVR trackers.

## Hardware Setup

To enable vibration motor support:

1. Connect a 3V vibration motor to a free GPIO pin on your ESP8266/ESP32
2. Configure the pin in `src/defines.h`:
   ```cpp
   #define VIBRATION_PIN 4  // Replace 4 with your chosen GPIO pin
   ```

### Recommended GPIO Pins

For ESP8266 (D1 Mini):
- GPIO 4 (D2)
- GPIO 5 (D1)
- GPIO 0 (D3) - Use with caution, affects boot mode
- GPIO 2 (D4) - Use with caution, affects boot mode

For ESP32:
- GPIO 4
- GPIO 5
- GPIO 16
- GPIO 17
- Other unused GPIO pins (check your board's pinout)

**Note:** Ensure the selected pin is not already used by IMU sensors, LED, or battery monitoring.

## Communication Protocol

The vibration feature uses the existing SlimeVR network protocol over UDP.

### Packet Structure

**Vibrate Packet (ReceivePacketType = 2)**

| Offset | Size | Type | Description |
|--------|------|------|-------------|
| 0-2 | 3 bytes | uint8[3] | Packet ID |
| 3 | 1 byte | uint8 | Packet Type (2 = Vibrate) |
| 4-11 | 8 bytes | uint64 (BE) | Packet Number |
| 12-13 | 2 bytes | uint16 (BE) | Duration in milliseconds |

**Total packet size:** 14 bytes

### Field Descriptions

- **Packet ID (bytes 0-2):** Standard SlimeVR packet identifier (3 bytes)
- **Packet Type (byte 3):** Must be `2` to indicate a Vibrate packet
- **Packet Number (bytes 4-11):** BigEndian uint64, standard packet sequencing
- **Duration (bytes 12-13):** BigEndian uint16 (0-65535 milliseconds)
  - `0`: Turn off vibration immediately
  - `1-65535`: Vibrate for specified milliseconds

### Behavior

- When a vibrate packet is received, the motor is activated for the specified duration
- The motor automatically turns off after the duration expires
- Sending a new vibrate command while vibrating will reset the timer with the new duration
- Sending duration `0` will immediately stop any ongoing vibration
- If `VIBRATION_PIN` is not configured (default 255), vibration commands are safely ignored

## Example Usage

### Python Example

```python
import socket
import struct

def send_vibrate(host, port, duration_ms):
    """
    Send a vibrate command to a SlimeVR tracker.
    
    Args:
        host: IP address of the tracker
        port: UDP port (default 6969)
        duration_ms: Duration in milliseconds (0-65535)
    """
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # Construct packet
    packet = bytearray(14)
    packet[0:3] = b'\x00\x00\x00'  # Packet ID
    packet[3] = 2                   # Packet Type (Vibrate)
    packet[4:12] = struct.pack('>Q', 0)  # Packet Number (BigEndian uint64)
    packet[12:14] = struct.pack('>H', duration_ms)  # Duration (BigEndian uint16)
    
    sock.sendto(packet, (host, port))
    sock.close()

# Example: Vibrate for 500ms
send_vibrate('192.168.1.100', 6969, 500)

# Example: Stop vibration
send_vibrate('192.168.1.100', 6969, 0)
```

### C++ Example

```cpp
#include <Arduino.h>
#include <WiFiUdp.h>

void sendVibrate(WiFiUDP& udp, IPAddress serverIP, uint16_t port, uint16_t durationMs) {
    uint8_t packet[14] = {0};
    
    // Packet ID (3 bytes)
    packet[0] = 0;
    packet[1] = 0;
    packet[2] = 0;
    
    // Packet Type (Vibrate = 2)
    packet[3] = 2;
    
    // Packet Number (8 bytes, BigEndian uint64)
    // Using 0 for simplicity
    for (int i = 4; i < 12; i++) {
        packet[i] = 0;
    }
    
    // Duration (2 bytes, BigEndian uint16)
    packet[12] = (durationMs >> 8) & 0xFF;
    packet[13] = durationMs & 0xFF;
    
    udp.beginPacket(serverIP, port);
    udp.write(packet, sizeof(packet));
    udp.endPacket();
}

// Usage:
// WiFiUDP udp;
// IPAddress trackerIP(192, 168, 1, 100);
// sendVibrate(udp, trackerIP, 6969, 500);  // Vibrate for 500ms
```

## Implementation Notes

- The vibration manager uses non-blocking updates in the main loop
- Pin control is implemented using standard Arduino `digitalWrite()`
- Duration is limited to 65535ms (approximately 65.5 seconds) by the uint16 data type
- The feature is designed to be minimal and efficient, suitable for haptic feedback in VR applications

## Troubleshooting

### Vibration motor doesn't activate
1. Verify `VIBRATION_PIN` is set to a valid GPIO pin in `defines.h`
2. Check that the pin is not used by other components
3. Ensure the motor is properly connected (check polarity if it's a DC motor)
4. Verify the ESP can provide enough current (some motors may need external power)
5. Check serial output for "Vibration motor initialized on pin X" message

### Motor stays on continuously
- Check for packet transmission errors
- Verify duration values are correct
- Send a vibrate command with duration `0` to turn it off

### No response to vibrate commands
- Verify network connectivity between server and tracker
- Check that packets are being sent to the correct IP and port
- Enable debug logging to see if packets are being received

## Safety Considerations

- Avoid very long vibration durations that might damage the motor or drain battery
- Consider adding thermal protection for continuous use scenarios
- Test power consumption impact on battery life
- Ensure motor noise doesn't interfere with IMU sensor readings
