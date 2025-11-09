// RS485 Interface
inline int PIN_RS485_RX = 16; // RS485 Receive Pin
inline int PIN_RS485_TX = 17; // RS485 Transmit Pin
inline int PIN_RS485_FLOW = 18; // RS485 Receiver Enable/Driver Enable Pin
inline int UART_RS485 = 2; // RS485 UART Assignment

// LocoNet Interface
inline int PIN_LN_RX = 4; // pin used to receive LocoNet signals
inline int PIN_LN_TX = 5; // pin used to transmit LocoNet signals
inline bool LN_RX_INVERT = false; // true is inverted signals
inline bool LN_TX_INVERT = true; // true is inverted signals
inline bool UART_LN = 1; // LocoNet UART Assignment

// Modbus Address Switches
inline int PIN_SW1 = 25; // Modbus Address Dipswitch Switch 1
inline int PIN_SW2 = 26; // Modbus Address Dipswitch Switch 2
inline int PIN_SW3 = 27; // Modbus Address Dipswitch Switch 3
inline int PIN_SW4 = 14; // Modbus Address Dipswitch Switch 4
inline int PIN_SW5 = 13; // Modbus Address Dipswitch Switch 5
inline int PIN_SW6 = 12; // Modbus Address Dipswitch Switch 6
inline int PIN_SW7 = 15; // Modbus Address Dipswitch Switch 7
inline int PIN_SW8 = 24; // Modbus Address Dipswitch Switch 8

// Onboard LEDs
inline int PIN_LED1 = 19; // LED 1
inline int PIN_LED2 = 21; // LED 2
inline int PIN_LED3 = 22; // LED 3
