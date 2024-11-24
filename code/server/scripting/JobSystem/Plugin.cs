namespace JobSystem
{
    public class Plugin
    {
        public static Plugin Instance { get; set; }
        internal JobManager JobManager { get; private set; }

        public float Time = 0.0f;
        public List<ulong> PlayerIds = new List<ulong>();

        static Plugin()
        {
            Instance = new Plugin();
        }

        Plugin()
        {
            JobManager = new JobManager();
        }
    }
}
