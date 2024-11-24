using System.Runtime.InteropServices;

internal class Program
{
  
    private static void SetupCallbacks()
    {
        CyberpunkMp.TUpdateCallback updateCallback = new CyberpunkMp.TUpdateCallback(Server.Loader.Server.Update);
        GCHandle.Alloc(updateCallback);
        CyberpunkMp.ServerAPI.SetUpdateCallback(updateCallback);

        CyberpunkMp.TPlayerEvent joinCallback = new CyberpunkMp.TPlayerEvent(Server.Loader.Server.PlayerJoin);
        GCHandle.Alloc(joinCallback);
        CyberpunkMp.ServerAPI.SetPlayerJoinCallback(joinCallback);

        CyberpunkMp.TPlayerEvent leaveCallback = new CyberpunkMp.TPlayerEvent(Server.Loader.Server.PlayerLeft);
        GCHandle.Alloc(leaveCallback);
        CyberpunkMp.ServerAPI.SetPlayerLeftCallback(leaveCallback);
    }

    private static void Main(string[] args)
    {
        try
        {
            CyberpunkMp.ServerAPI.Initialize();

            SetupCallbacks();

            CyberpunkMp.ServerAPI.Run();
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
    }
}