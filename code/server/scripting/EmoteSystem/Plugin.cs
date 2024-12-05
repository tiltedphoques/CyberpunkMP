using CyberpunkSdk;
using EmbedIO.WebApi;

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
                _lastEmoteLock.EnterReadLock();
                var lastEmote = _lastEmote;
                _lastEmoteLock.ExitReadLock();
                return lastEmote;
            }
        }
        private EmoteDto? _lastEmote;
        private readonly ReaderWriterLockSlim _lastEmoteLock = new();

        private Plugin()
        {
        }

        public void UpdateLastEmote(string username, string emote)
        {
            _lastEmoteLock.EnterWriteLock();
            _lastEmote = new EmoteDto(username, emote);
            _lastEmoteLock.ExitWriteLock();
        }

        public Func<PluginWebApiController> BuildController()
        {
            return () => new EmoteController(Instance);
        }
    }
}