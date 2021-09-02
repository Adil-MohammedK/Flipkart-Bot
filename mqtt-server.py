"""
Python MQTT Subscription client
Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
Written for my Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
"""
import paho.mqtt.client as mqtt
from flask import Flask, render_template, request

app = Flask(__name__)

# Don't forget to change the variables for the MQTT broker!
mqtt_username = "AdilPi"
mqtt_password = "Adilap268"
# mqtt_topic = "esp/led_control"
mqtt_broker_ip = "192.168.1.40"

pins = {
    4: {"name": "Forward", "board": "esp8266", "topic": "esp8266/F", "state": "False"},
    5: {"name": "Back", "board": "esp8266", "topic": "esp8266/B", "state": "False"},
    6: {"name": "Right", "board": "esp8266", "topic": "esp8266/R", "state": "False"},
    7: {"name": "Left", "board": "esp8266", "topic": "esp8266/L", "state": "False"},
}

# Put the pin dictionary into the template data dictionary:
templateData = {"pins": pins}

client = mqtt.Client()
# Set the username and password for the MQTT client
client.username_pw_set(mqtt_username, mqtt_password)
client.connect("192.168.1.40", 1883, 60)
client.loop_start()


@app.route("/")
def main():
    client.publish(pins[4]["topic"], "1")
    # Pass the template data into the template main.html and return it to the user
    return render_template("main.html", **templateData)


# The function below is executed when someone requests a URL with the pin number and action in it:
@app.route("/<board>/<changePin>/<action>")
def action(board, changePin, action):
    # Convert the pin from the URL into an integer:
    changePin = int(changePin)
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

    return render_template("main.html", **templateData)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8181, debug=True)
