# GoogleIoTSchool2023
This is the repository of the work done at the 2023 Google Summer School in IoT that happened at UPB. All projects require an ESP32 to compile to and work. Additional sensors required per project.
Here is a short summary of every project listed in their respective directories:

### agenda_bt
Establishes a connection through Bluetooth with another device and uses predefined commands to store text into the flash storage of the ESP32.

### cmmdc_nfc
Reads NFC compatible cards for their coded numbers, and calculcates the GCD of all, which compares with a predefined value to see if they are equal. This can be used for example in a "escape room" scenario, where each card can unlock clues or the door to escape.

### coap_led
Using the COAP protocol, it talks to a website that controls a LED that can be toggled on/off.

### ifttt
Using the IFTTT service and a DHT22, it periodically records the room temperature and writes it down in a Google Spreadsheet.

### led_temp
Using a DHT22, it actives a LED past a threshold.

### led_test_esp32
Just lights a LED. Nothing special.

### mqtt_led
Using the MQTT protocol, it listens to a server to check for messages to be received in order to toggle a LED. The Python script generates the response the ESP32 should listen to.

### ventilator_project
Using a DHT22 and a humidity sensor, it sends the data to a master control unit (in this case another ESP32 board) that checks whether the home system should be active or not. If it is, it then actives a motor (in this example is used a servo, as there was a lack of a DC motor with an actual fan) until a shutdown signal is received. It is part of the last day of the camp program, and it works in join with another EPS32 that transmits the commands.

### wifi_server
Connects to the internet and sets up a HTTP web server that displays a website.
