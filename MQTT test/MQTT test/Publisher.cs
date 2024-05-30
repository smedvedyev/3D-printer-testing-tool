using System;
using System.Reflection;
using System.Text;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Options;
using MQTTnet.Server;

class Publisher
{
    static async Task Main(string[] args)
    {
        var mqtt = new MqttFactory();
        var client = mqtt.CreateMqttClient();
        var options = new MqttClientOptionsBuilder()
            .WithClientId(Guid.NewGuid().ToString())
            .WithTcpServer("test.mosquitto.org", 1883)
            .WithCleanSession(true)
            .Build();

        client.UseConnectedHandler(handler =>
        {
            Console.WriteLine("Connected to the broker successfully");

        });
        client.UseDisconnectedHandler(handler =>
        {
            Console.WriteLine("Disconnected");

        });
        await client.ConnectAsync(options);

        await PublishMessageAsync(client);
        Console.ReadLine();
    }

    private static async Task PublishMessageAsync(IMqttClient client)
    {
        while (true)
        {
            Console.WriteLine("Type message:");
            string messagePayload = Console.ReadLine();
            var message = new MqttApplicationMessageBuilder()
                .WithTopic("inTopic")
                .WithPayload(messagePayload)
                .WithAtLeastOnceQoS()
                .Build();

            if (client.IsConnected)
            {
                if (messagePayload == "disconnect")
                {
                    await client.DisconnectAsync();
                }
                else
                {
                    await client.PublishAsync(message);
                    Console.WriteLine($"Published message - {messagePayload}");

                }
            }

        }

    }

}

