using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;




namespace WpfApp1
{
    public class Client
    {

        //===========================================
        // CONSTANTS
        //===========================================

        // The url address of MQTT broker.
        const string BROKER_URL = "test.mosquitto.org";

        //===========================================
        // GLOBAL VARIABLES
        //===========================================

        MqttClient client;
        ListBox console;
        TextBox tbHum;
        TextBox tbTmp;

        string message;

        //===========================================
        // PROTOTYPES AND DELEGATES
        //===========================================

        // Helpers for ListBox control.
        public delegate void AddListItem();
        public AddListItem myDelegate;

        //===========================================
        // CONSTRUCTORS AND DESTRUCTOR
        //===========================================

        /// <summary>
        /// Instantiate a client with specified listbox to print messages.
        /// </summary>
        /// <param name="console">The listbox to print messages.</param>
        public Client(ListBox console, TextBox tbHum, TextBox tbTmp)
        {
            this.console = console;
            this.tbHum = tbHum;
            this.tbTmp = tbTmp;
            myDelegate = new AddListItem(AddListItemMethod);
            //
            // Instantiate MQTT client object.
            //
            //MqttClient client = new MqttClient("broker.hivemq.com");
            client = new MqttClient(BROKER_URL);
            //
            // Set the events. Add them if necessary.
            //
            client.MqttMsgPublished += Client_MqttMsgPublished;
            client.MqttMsgPublishReceived += Client_MqttMsgPublishReceived;
            client.MqttMsgSubscribed += Client_MqttMsgSubscribed;
            client.MqttMsgUnsubscribed += Client_MqttMsgUnsubscribed;
            //
            // Connect to MQTT broker with a unique id.
            //
            byte code = client.Connect(Guid.NewGuid().ToString());
            console.Items.Add(code);
            //
            // Lets subscribe to receive something from a topic.
            //
            ushort msgId = client.Subscribe(new string[] { "studentinc/acme/xyzManipulator" },
                   new byte[] { MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE });
            console.Items.Add(msgId);

            ushort msgId2 = client.Subscribe(new string[] { "studentinc/acme/environment/hum" },
                   new byte[] { MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE });
            console.Items.Add(msgId2);

            ushort msgId3 = client.Subscribe(new string[] { "studentinc/acme/environment/temp" },
            new byte[] { MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE });
            console.Items.Add(msgId3);

            //
            // Lets publish something to a topic on the MQTT broker.
            //

        }

        /// <summary>
        /// Destruct the client.
        /// </summary>
        ~Client()
        {
            if (client.IsConnected)
            {
                client.Disconnect();
            }
        }

        //===========================================
        // METHODS
        //===========================================
        public void PublishOnClick(string message, string topic)
        {
            ushort msgId = client.Publish(topic, // topic
            Encoding.UTF8.GetBytes(message), // message body, empty string clears the topic.
            MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, // QoS level
            true); // retained

            console.Items.Add(msgId);

        }
        /// <summary>
        /// Unsubscribe the client from all subscribed topics.
        /// </summary>
        public void Unsubscribe()
        {
            client.Unsubscribe(new string[] { "studentinc/acme/xyzManipulator" });
            client.Unsubscribe(new string[] { "studentinc/acme/environment" });
            client.Unsubscribe(new string[] { "studentinc/acme/environment/hum" });
            client.Unsubscribe(new string[] { "studentinc/acme/environment/temp" });
        }

        /// <summary>
        /// Disconnect from the mqtt broker.
        /// </summary>
        public void Disconnect()
        {
            if (client.IsConnected)
            {
                client.Disconnect();
            }
        }

        private void AddListItemMethod()
        {
            console.Items.Add(message);
        }

        //===========================================
        // EVENTS HANDLING METHODS
        //===========================================

        void Client_MqttMsgSubscribed(object sender, MqttMsgSubscribedEventArgs e)
        {
            message = "Subscribed " + e.MessageId;
            console.Dispatcher.Invoke(myDelegate);
        }

        private void Client_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            if (e.Topic == "studentinc/acme/environment/hum")
            {

                message = Encoding.Default.GetString(e.Message);
                UpdateValue(message, "hum");
                console.Dispatcher.Invoke(myDelegate);
            }
            else if (e.Topic == "studentinc/acme/environment/temp")
            {

                message = Encoding.Default.GetString(e.Message);
                UpdateValue(message, "temp");
                console.Dispatcher.Invoke(myDelegate);

            }
            else if (e.Topic == "studentinc/acme/xyzManipulator")
            {
                message = Encoding.Default.GetString(e.Message);
                console.Dispatcher.Invoke(myDelegate);
            }

        }

        private void Client_MqttMsgPublished(object sender, MqttMsgPublishedEventArgs e)
        {
            message = "Published " + e.MessageId.ToString();
            console.Dispatcher.Invoke(myDelegate);
        }

        private void Client_MqttMsgUnsubscribed(object sender, MqttMsgUnsubscribedEventArgs e)
        {
            // Cannot print to console, because console is closed at FormClosed().
        }
        private void UpdateValue(string message, string dataType)
        {
            switch (dataType)
            {
                case "hum":
                    if (tbHum.Dispatcher.CheckAccess())
                    {
                        tbHum.Dispatcher.Invoke(new Action<string, string>(UpdateValue), message, "hum");
                    }
                    else
                    {
                        tbHum.Text = message;
                    }

                    break;
                case "temp":
                    if (tbTmp.Dispatcher.CheckAccess())
                    {
                        tbTmp.Dispatcher.Invoke(new Action<string, string>(UpdateValue), message, "temp");
                    }
                    else
                    {
                        tbTmp.Text = message;
                    }
                    break;
            }
        }

    }
}
