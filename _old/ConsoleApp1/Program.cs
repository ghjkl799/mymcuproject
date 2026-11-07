
//spectre.console
//https://www.nuget.org/packages/RJCP.SerialPortStream



using System.IO.Ports;
using System.Text;

namespace ConsoleApp1;

class Program
{
    public static async Task Main(string[] args)
    {
        //const int baudRate = 9600;
        const int baudRate = 115200;

        CancellationTokenSource cts = new CancellationTokenSource();
        //cts.CancelAfter(10000);
        var controller = await Controller.Create(baudRate, Console.WriteLine, cts.Token) ?? throw new Exception();

        bool led_toggle = false;

        var command = new Command();

        while (true)
        {
            //Console.Write("send command: ");
            //var command = Console.ReadLine();
            //if (string.IsNullOrWhiteSpace(command)) continue;
            //controller.Send(command);

            command = command with { DebugOn = true, LedOn = !command.LedOn };
            controller.Send(command);

            await Task.Delay(5000);
        }


        //if (controller == null)
        //{
        //    Console.WriteLine("Could not connect. Stopping.");
        //    return;
        //}
        //Console.WriteLine("Connected.");






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

public record Command(bool DebugOn = false, bool LedOn = true)
{
    public override string ToString()
    {
        string[] arr = [
            DebugOn ? "DEBUG_ON" : "DEBUG_OFF",
            LedOn ? "LED_ON" : "LED_OFF"
        ];
        return string.Join(';', arr);
    }
}


public class Controller : IAsyncDisposable
{
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

    public void Send(string command)
    {
        WriteTo($"Sending command: {command}");
        Port.WriteLine(command);
    }

    public void Send(Command command)
    {
        WriteTo($"Sending command: {command}");
        Port.WriteLine(command.ToString());
    }



    public async ValueTask DisposeAsync()
    {
        await Task.Run(() =>
        {
            Port.Close();
            Port.Dispose();
        });
    }
}

