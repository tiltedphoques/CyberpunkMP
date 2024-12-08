using CyberpunkSdk;

namespace EmoteSystem
{
    public class Plugin : IWebApiHook
    {
        public static Plugin Instance { get; set; }

        static Plugin()
        {
            Instance = new Plugin();
        }

        public EmoteDto? LastEmote
        {
            get
            {
                lock (_emoteLock)
                {
                    return _lastEmote;
                }
            }
            private set
            {
                lock (_emoteLock)
                {
                    _lastEmote = value;
                }
            }
        }

        private EmoteDto? _lastEmote;
        private readonly object _emoteLock = new();

        private Plugin()
        {
        }

        public void UpdateLastEmote(string username, string emote)
        {
            LastEmote = new EmoteDto(username, emote);
        }

        public Func<PluginWebApiController> BuildController()
        {
            return () => new EmoteController(Instance);
        }
    }
}