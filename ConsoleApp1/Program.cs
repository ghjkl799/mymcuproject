using System.IO.Ports;

const int baudRate = 115200;

CancellationTokenSource cts = new CancellationTokenSource();
cts.CancelAfter(10000);
var controller = await Controller.Create(baudRate, cts.Token);

if(controller == null)
{
    Console.WriteLine("Could not connect. Stopping.");
    return;
}
Console.WriteLine("Connected.");

//controller.EchoOn();
//controller.BlinkOn();

////controller.LedOff();
////await Task.Delay(1000);
////controller.LedOn();
////await Task.Delay(1000);
////controller.LedOff();
////await Task.Delay(1000);
////controller.LedOn();
////await Task.Delay(1000);

//for (var i = 0; i < 10; i++)
//{
//    controller.ReadTemperature();
//    await Task.Delay(1000);
//    controller.ReadHumidity();
//    await Task.Delay(1000);
//}




//foreach (var str in controller.Read())
//{
//    var d = DateTime.Now.ToLongTimeString();
//    Console.WriteLine($"{d} {str}");
//}

Console.ReadLine();



class Controller
{
    const string CLedOn = "LED_ON";
    const string CLedOff = "LED_OFF";
    const string CEchoOn = "ECHO_ON";
    const string CEchoOff = "ECHO_OFF";
    const string CBlinkOn = "BLINK_ON";
    const string CBlinkoff = "BLINK_OFF";
    const string CReadTemperature = "READ_TEMPERATURE";
    const string CReadHumidity = "READ_HUMIDITY";

    public static async Task<Controller?> Create(int baudRate, CancellationToken ct = default)
    {
        while (!ct.IsCancellationRequested)
        {
            string? portName = SerialPort.GetPortNames().FirstOrDefault();
            if (portName == null)
            {
                Console.WriteLine("Port not available.");
                await Task.Delay(1000);
                continue;
            }

            Console.WriteLine("Available port: " + portName);
            SerialPort port = new(portName, baudRate);
            port.Open();

            return new Controller(port);
        }
        return null;
    }

    SerialPort Port;
    private Controller(SerialPort port)
    {
        Port = port;
        Port.DataReceived += Port_DataReceived;
    }

    private void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
    {
        while(Port.BytesToRead > 0)
        {
            Console.WriteLine(Port.ReadLine());
        }
    }

    public IEnumerable<string> Read(CancellationToken ct = default)
    {
        while(!ct.IsCancellationRequested)
        {
            yield return Port.ReadLine();
        }
    }

    private void SendCommand(string command)
    {
        Console.WriteLine($"Sending command: {command}");
        Port.WriteLine(command);
    }

    public void LedOn() => SendCommand(CLedOn);
    public void LedOff() => SendCommand(CLedOff);
    public void EchoOn() => SendCommand(CEchoOn);
    public void EchoOff() => SendCommand(CEchoOff);
    public void BlinkOn() => SendCommand(CBlinkOn);
    public void BlinkOff() => SendCommand(CBlinkoff);
    public void ReadTemperature() => SendCommand(CReadTemperature);
    public void ReadHumidity() => SendCommand(CReadHumidity);
}

