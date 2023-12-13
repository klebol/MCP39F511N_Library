# MCP39F511N_Library
Library for MCP39F11 - Power Monitoring IC <br>
Library consists two files pair - mcp39f511n.c/h and PowerMeasurement.c/.h <br>
mcp39f511n files are responsible for low level communication with IC via uart <br>
PowerMeasurement is an API libraru for mcp39f511n, wchich was designed to work in a main loop <br>
To set up the library you need to : <br>
1. Define a AC_Parameters_t variable for keeping received data
2. Init the IC via PowerMeasurement_Init functuion (you need to pass a proper UART handle)
3. Put a PowerMesurement_RX_Callback in the HAL's UART Rx Event callback
4. Call a PowerMeasurement_Process in while loop
