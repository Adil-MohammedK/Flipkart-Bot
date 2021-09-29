import paho.mqtt.client as mqtt
import passwords

# Don't forget to change the variables for the MQTT broker!
mqtt_username = "AdilPi"
mqtt_password = passwords.mqtt_pass
# mqtt_topic = "esp/led_control"
mqtt_broker_ip = passwords.mqtt_ip

pins = {
    "F": {"name": "Forward", "board": "esp8266", "topic": "esp8266/F", "state": "False"},
    "B": {"name": "Back", "board": "esp8266", "topic": "esp8266/B", "state": "False"},
    "R": {"name": "Right", "board": "esp8266", "topic": "esp8266/R", "state": "False"},
    "L": {"name": "Left", "board": "esp8266", "topic": "esp8266/L", "state": "False"},
}

# Put the pin dictionary into the template data dictionary:
templateData = {"pins": pins}

client = mqtt.Client()
# Set the username and password for the MQTT client
client.username_pw_set(mqtt_username, mqtt_password)
client.connect(mqtt_broker_ip, 1883, 60)
client.loop_start()


def main():
    client.publish(pins["F"]["topic"], "0")
    client.publish(pins["R"]["topic"], "0")
    client.publish(pins["L"]["topic"], "0")
    client.publish(pins["B"]["topic"], "0")
    # Pass the template data into the template main.html and return it to the user


# The function below is executed when someone requests a URL with the pin number and action in it:
def action(board, changePin, action):
    # Convert the pin from the URL into an integer:
    # changePin = int(changePin)
    # Get the device name for the pin being changed:
    devicePin = pins[changePin]["name"]
    # If the action part of the URL is "on," execute the code indented below:
    if action == "1" and board == "esp8266":
        client.publish(pins[changePin]["topic"], "1")
        pins[changePin]["state"] = "True"

    if action == "0" and board == "esp8266":
        client.publish(pins[changePin]["topic"], "0")
        pins[changePin]["state"] = "False"

    # Along with the pin dictionary, put the message into the template data dictionary:
    templateData = {"pins": pins}


main()
file = open("dirData.txt", "r")
str = file.readline()
# print(file)
for x in str:
    action("esp8266", x, "1")
    print(x)
file.close()
