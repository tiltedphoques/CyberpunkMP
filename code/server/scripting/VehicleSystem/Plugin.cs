using CyberpunkSdk;

namespace EmoteSystem
{
    public class Plugin
    {
        public static Plugin Instance { get; set; }

        static Plugin()
        {
            Instance = new Plugin();
        }

        Plugin()
        {
        }
    }
}
