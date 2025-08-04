using ConsoleApp1;

namespace WinFormsApp1
{
    public partial class Form1 : Form
    {
        Controller? Controller;

        bool SerialPortConnected; 

        public Form1()
        {
            InitializeComponent();
            StateHasChanged();
        }

        void StateHasChanged()
        {
            SerialPortConnected = Controller != null;


            button1.Text = SerialPortConnected ? "Stop serial port" : "Start serial port";
            label2.Text = SerialPortConnected ? "connected" : "disconnected";
        }

        async Task CreateController()
        {
            const int baudRate = 115200;
            Controller = await Controller.Create(baudRate, WriteToTextbox);
        }

        void WriteToTextbox(string s)
        {
            Invoke(() => textBox1.Lines = [s.Trim(), .. textBox1.Lines.Take(9)]);
        }

        private async void button1_Click(object sender, EventArgs e)
        {
            if (SerialPortConnected)
            {
                if (Controller != null)
                {
                    await Controller.DisposeAsync();
                    Controller = null;
                }
                textBox1.Text = string.Empty;
            }
            else
            {
                await CreateController();
            }
            StateHasChanged();
        }
    }
}
