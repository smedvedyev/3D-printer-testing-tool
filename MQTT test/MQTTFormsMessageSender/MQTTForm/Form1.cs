using MQTTclient;

namespace MQTTForm
{
    public partial class Form1 : Form
    {
        Client client;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            client = new Client(console);
        }

        private void btnLedOn_Click(object sender, EventArgs e)
        {
            client.PublishOnClick("1N");
        }

        private void btnLedOff_Click(object sender, EventArgs e)
        {
            client.PublishOnClick("1FF");

        }

        private void btnYellowLedOn_Click(object sender, EventArgs e)
        {
            client.PublishOnClick("2N");
        }

        private void btnYellowLedOff_Click(object sender, EventArgs e)
        {
            client.PublishOnClick("2FF");
        }
    }
}