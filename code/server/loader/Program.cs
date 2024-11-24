using CyberpunkSdk;
using System.Runtime.InteropServices;
using System.Security;

internal class Program
{
    private static void Main(string[] args)
    {
        try
        {
            CyberpunkMp.ServerAPI.Initialize();
            CyberpunkMp.ServerAPI.Run();
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
    }
}