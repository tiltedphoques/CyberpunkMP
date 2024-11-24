using CyberpunkSdk;
using CyberpunkSdk.Systems;
using CyberpunkSdk.Types;


namespace Cyberpunk.Rpc.Server.Plugins
{
    public static partial class JobsServer
    {
        public static partial void SelectJob_Impl(ulong playerId, CName job)
        {
            JobSystem.Plugin.Instance.JobManager.SetJob(playerId, job);
        }

        public static partial void QuitJob_Impl(ulong playerId)
        {
            JobSystem.Plugin.Instance.JobManager.Quit(playerId);
        }
    }
}

namespace JobSystem
{
    internal class JobManager
    {
        private Dictionary<ulong, Job> playerJobs = new Dictionary<ulong, Job>();
        private Dictionary<CName, Func<ulong, Job>> availableJobs = new Dictionary<CName, Func<ulong, Job>>();

        private Logger logger = new Logger("Jobs");

        internal JobManager()
        {
            Server.PlayerSystem.PlayerJoinEvent += OnPlayerJoin;
            Server.PlayerSystem.PlayerLeftEvent += OnPlayerLeave;

            Server.World.UpdateEvent += Update;

            RegisterJob<DeliveryDriver>();
            RegisterJob<Police>();
            RegisterJob<Taxi>();

            Server.Timer.Add(Save, 300.0f);
            Server.Timer.Add(HeavyUpdate, 0.5f);
        }

        private void RegisterJob<T>() where T : Job
        {
            var jobCName = typeof(T).GetField("Job", System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static)?.GetValue(null) as CName;

            if(jobCName == null)
            {
                logger.Error("Attempt to register job without a name.");
                return;
            }

            availableJobs[jobCName] = (ulong id) => Activator.CreateInstance(typeof(T), [id]) as Job;

            logger.Info("\tRegistered job " + jobCName);
        }

        internal bool Get(ulong id, out Job? job)
        {
            return playerJobs.TryGetValue(id, out job);
        }

        public void SetJob(ulong id, CName name)
        {
            if(playerJobs.TryGetValue(id, out var jobName))
            {
                logger.Warn($"Player {id} tried to get new job {name} but already has a job {jobName}!");
                Cyberpunk.Rpc.Client.Plugins.JobsClient.SetActiveJob(id, jobName.Name);
                return;
            }

            if(availableJobs.TryGetValue(name, out var creator))
            {
                var job = creator(id);
                playerJobs.Add(id, job);
            }
            else
                logger.Warn($"Player {id} tried to get new job {name} but it doesn't exist!");

            logger.Info($"Player {id} got new job {name}.");

            Cyberpunk.Rpc.Client.Plugins.JobsClient.SetActiveJob(id, name);
        }

        public void Quit(ulong id)
        {
            if(playerJobs.Remove(id, out var job))
            {
                logger.Info($"Player {id} quit job {job.Name}.");

                Cyberpunk.Rpc.Client.Plugins.JobsClient.SetActiveJob(id, new("None"));
            }
        }

        private void Update(float delta)
        {
            foreach (var job in playerJobs.Values)
            {
                job.Update(delta);
            }
        }

        private void HeavyUpdate(float delta)
        {
            foreach (var job in playerJobs.Values)
            {
                job.HeavyUpdate(delta);
            }
        }

        private void OnPlayerJoin(ulong id)
        {
            // Load from db
        }

        private void OnPlayerLeave(ulong id)
        {
            if (playerJobs.ContainsKey(id))
            {
                Save(0.0f);
                playerJobs.Remove(id);
            }
        }

        private void Save(float delta)
        {
            logger.Info("Saving Jobs...");
        }
    }
}
