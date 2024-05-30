using System;
using System.Reflection;
using System.Text;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Options;

using MQTTnet.Server;


class Subscriber
{
    static async Task Main(string[] args)
    {
        var mqtt = new MqttFactory();
        IMqttClient client = mqtt.CreateMqttClient();
        var options = new MqttClientOptionsBuilder()
            .WithClientId(Guid.NewGuid().ToString())
            .WithTcpServer("test.mosquitto.org", 1883)
            .WithCleanSession(true)
            .Build();

        client.UseConnectedHandler(async handler =>
        {
            Console.WriteLine("Connected to the broker successfully");
            var topicFilter = new TopicFilterBuilder()
            .WithTopic("SM")
            .Build();
            client.SubscribeAsync(topicFilter).Wait();

        });
        client.UseDisconnectedHandler(handler =>
        {
            Console.WriteLine("Disconnected");

        });

        client.UseApplicationMessageReceivedHandler(e =>
        {
            Console.WriteLine($"Received message - {Encoding.UTF8.GetString(e.ApplicationMessage.Payload)}");
        });

        await client.ConnectAsync(options);
        Console.ReadLine();
        await client.DisconnectAsync();
    }

}

