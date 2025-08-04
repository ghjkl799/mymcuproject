
//spectre.console
//https://www.nuget.org/packages/RJCP.SerialPortStream



using System.IO.Ports;

namespace ConsoleApp1;

class Program
{
    public static async Task Main(string[] args)
    {

        //const int baudRate = 9600;
        const int baudRate = 115200;

        CancellationTokenSource cts = new CancellationTokenSource();
        cts.CancelAfter(10000);
        var controller = await Controller.Create(baudRate, null, cts.Token);

        if (controller == null)
        {
            Console.WriteLine("Could not connect. Stopping.");
            return;
        }
        Console.WriteLine("Connected.");






        //controller.EchoOn();
        //controller.BlinkOn();

        //controller.LedOff();
        //await Task.Delay(1000);
        //controller.LedOn();
        //await Task.Delay(1000);
        //controller.LedOff();
        //await Task.Delay(1000);
        //controller.LedOn();
        //await Task.Delay(1000);

        //for (var i = 0; i < 10000; i++)
        //{

        //    controller.ReadTemperature();
        //    await Task.Delay(3000);
        ////controller.ReadTemperature();
        ////await Task.Delay(3000);
        ////controller.ReadTemperature();
        ////await Task.Delay(3000);
        ////controller.ReadHumidity();
        ////await Task.Delay(1000);

        //}




        //foreach (var str in controller.Read())
        //{
        //    var d = DateTime.Now.ToLongTimeString();
        //    Console.WriteLine($"{d} {str}");
        //}

        Console.ReadLine();
    }
}



public class Controller : IAsyncDisposable
{
    const string CLedOn = "LED_ON";
    const string CLedOff = "LED_OFF";
    const string CEchoOn = "ECHO_ON";
    const string CEchoOff = "ECHO_OFF";
    const string CBlinkOn = "BLINK_ON";
    const string CBlinkoff = "BLINK_OFF";
    const string CReadTemperature = "READ_TEMPERATURE";
    const string CReadHumidity = "READ_HUMIDITY";

    private Action<string> WriteTo;

    public static async Task<Controller?> Create(int baudRate, Action<string>? writeTo = null, CancellationToken ct = default)
    {
        writeTo ??= s => { };
        while (!ct.IsCancellationRequested)
        {
            string? portName = SerialPort.GetPortNames().FirstOrDefault();
            if (portName == null)
            {
                writeTo("Port not available.");
                await Task.Delay(1000);
                continue;
            }

            writeTo("Available port: " + portName);
            SerialPort port = new(portName, baudRate);
            port.Open();


            return new Controller(port, writeTo);
        }
        return null;
    }

    SerialPort Port;
    private Controller(SerialPort port, Action<string> writeTo)
    {
        Port = port;
        Port.DataReceived += Port_DataReceived;
        WriteTo = writeTo;
    }

    private void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
    {
        while(Port.BytesToRead > 0)
        {
            WriteTo(Port.ReadLine());
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
        WriteTo($"Sending command: {command}");
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

    public async ValueTask DisposeAsync()
    {
        await Task.Run(() =>
        {
            Port.Close();
            Port.Dispose();
        });
    }
}

