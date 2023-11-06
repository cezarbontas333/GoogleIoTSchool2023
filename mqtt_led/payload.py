import paho.mqtt.client as mqtt # Import the MQTT library
import time # The time library is useful for delays


# Our "on message" event
def messageFunction (client, userdata, message):
    topic = str(message.topic)

    message = str(message.payload.decode("utf-8"))

    print(topic + message)

myClient = mqtt.Client("EdgeLord_cevaIoT") # Create a MQTT client object

myClient.connect("test.mosquitto.org", 1883) # Connect to the test MQTT broker

myClient.subscribe("niceedgelord_led") # Subscribe to the topic AC_unit

myClient.on_message = messageFunction # Attach the messageFunction to subscription

myClient.loop_start() # Start the MQTT client


# Main program loop
while(1):
    myClient.publish("niceedgelordpc", "1") # Publish message to MQTT broker
    time.sleep(1) # Sleep for a second
    myClient.publish("niceedgelordpc", "0") # Publish message to MQTT broker
    time.sleep(1) # Sleep for a second
