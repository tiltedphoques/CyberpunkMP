using Cyberpunk.Rpc.Plugins;
using Cyberpunk.Rpc.Server.Plugins;
using CyberpunkSdk;
using CyberpunkSdk.Internal;
using CyberpunkSdk.Systems;
using CyberpunkSdk.Types;
using JobSystem;

namespace Cyberpunk.Rpc.Server.Plugins
{
    public static partial class TaxiServer
    {
        internal struct JobData
        {
            internal uint Id;
            internal ulong PlayerId;
            internal Vector4 PickUpLocation;
            internal Vector4 Location;
            internal uint Reward;
        }

        private static uint _jobId = 0;
        internal static Dictionary<uint, JobData> AvailableJobs = new Dictionary<uint, JobData>();

        public static partial void CreateJob_Impl(ulong playerId, Vector4 location)
        {
            var jobManager = Plugin.Instance.JobManager;

            var player = CyberpunkSdk.Server.PlayerSystem.GetById(playerId);

            var playerLocation = new Vector4();
            playerLocation.X = player.MovementComponent.Position.X;
            playerLocation.Y = player.MovementComponent.Position.Y;
            playerLocation.Z = player.MovementComponent.Position.Z;
            playerLocation.W = 0;

            var id = ++_jobId;
            AvailableJobs.Add(id, new JobData { Id = id, PlayerId = playerId, PickUpLocation = playerLocation, Location = location, Reward = 100 });
        }

        public static partial void AcceptJob_Impl(ulong playerId, uint id)
        {
            var jobManager = Plugin.Instance.JobManager;
            if (jobManager.Get(playerId, out Job? job) && job is Taxi)
            {
                var delivery = job as Taxi;
                delivery!.AcceptJob(id);
            }
        }

        public static partial void CancelJob_Impl(ulong playerId)
        {
            var jobManager = Plugin.Instance.JobManager;
            if (jobManager.Get(playerId, out Job? job) && job is Taxi)
            {
                var delivery = job as Taxi;
                delivery!.CancelJob();
            }
        }

        public static partial void LoadJobs_Impl(ulong playerId)
        {
            var jobManager = Plugin.Instance.JobManager;
            if (jobManager.Get(playerId, out Job? job) && job is Taxi)
            {
                var delivery = job as Taxi;
                delivery!.LoadJobs();
            }
        }

        public static partial void RiderReady_Impl(ulong playerId)
        {
            var jobManager = Plugin.Instance.JobManager;
            if (jobManager.Get(playerId, out Job? job) && job is Taxi)
            {
                var delivery = job as Taxi;
                delivery!.RiderReady();
            }
        }

        static internal JobData? AssignJob(uint id)
        {
            if (AvailableJobs.TryGetValue(id, out var job))
            {
                AvailableJobs.Remove(id);
                return job;
            }

            return null;
        }
    }
}

namespace JobSystem
{
    internal class Taxi : Job
    {
        public static readonly CName Job = new("Taxi Driver");
        private static Logger logger = new Logger("Taxi");

        public override CName Name { get { return Job; } }

        private TaxiServer.JobData? activeJob = null;
        private bool isRiderReady = false;

        public Taxi(ulong id) : base(id)
        {
        }

        ~Taxi()
        {
            CancelJob();
        }

        public override void Update(float delta)
        {
        }

        public override void HeavyUpdate(float delta)
        {
            if (activeJob == null)
                return;

            if(!isRiderReady)
            {
                // Check if rider is in the vehicle
                var driver = Server.PlayerSystem.GetById(Id);
                if (driver == null)
                {
                    logger.Error($"Player {Id} doesn't exist?");
                    CancelJob();
                    return;
                }

                var rider = Server.PlayerSystem.GetById(activeJob.Value.PlayerId);
                if (rider == null)
                {
                    driver.SendChat("SERVER", $"Rider disappeared...");
                    CancelJob();
                    return;
                }

                if (driver.AttachmentComponent.Parent != 0 &&
                    driver.AttachmentComponent.Parent == rider.AttachmentComponent.Parent)
                {
                    RiderReady();
                }
            }
            else
            {
                var player = Server.PlayerSystem.GetById(Id);
                var pos = player.MovementComponent.Position;
                var speed = player.MovementComponent.Velocity;
                var target = activeJob.Value.Location;

                var distance = System.Numerics.Vector3.DistanceSquared(new System.Numerics.Vector3(pos.X, pos.Y, pos.Z), new System.Numerics.Vector3(target.X, target.Y, target.Z));

                if (distance < 1000.0f && speed < 1.0)
                {
                    var rider = CyberpunkSdk.Server.PlayerSystem.GetById(activeJob.Value.PlayerId);
                    if (rider != null)
                    {
                        // Wait for player to exit vehicle
                        if(rider.AttachmentComponent.Parent == player.AttachmentComponent.Parent)
                        {
                            return;
                        }
                        rider.SendChat("SERVER", $"Taxi ride complete! You paid {activeJob.Value.Reward}");
                    }

                    Cyberpunk.Rpc.Client.Plugins.TaxiDriverClient.EndTrip(Id, true, activeJob.Value.Reward);
                    Cyberpunk.Rpc.Client.Plugins.TaxiRiderClient.EndTrip(activeJob.Value.PlayerId, true, activeJob.Value.Reward);

                    player.SendChat("SERVER", $"Taxi job complete! You earned {activeJob.Value.Reward}");

                    activeJob = null;
                    isRiderReady = false;
                }
                else
                {
                    logger.Info($"Distance: {distance}, Speed: {speed}");
                }
            }
        }

        public void CancelJob()
        {
            var job = activeJob;
            isRiderReady = false;

            if (job != null)
            {
                Cyberpunk.Rpc.Client.Plugins.TaxiDriverClient.EndTrip(Id, false, 0);

                TaxiServer.AvailableJobs.Add(job.Value.Id, job.Value);

                activeJob = null;

                var player = Server.PlayerSystem.GetById(Id);
                if(player != null)
                    player.SendChat("SERVER", $"Taxi job cancelled");
            }
        }

        public void AcceptJob(uint jobId)
        {
            if (activeJob != null)
            {
                logger.Warn($"Attempt from player {Id} to accept job while job is active!");
                return;
            }

            var job = TaxiServer.AssignJob(jobId);
            if (job == null)
            {
                logger.Warn($"Attempt from player {Id} to accept job already accepted!");
                return;
            }

            activeJob = job;

            var player = Server.PlayerSystem.GetById(Id);
            player.SendChat("SERVER", $"Taxi job accepted!");
            player.SendChat("SERVER", $"Drive to the pick up location and wait for the rider to sit.");

            Cyberpunk.Rpc.Client.Plugins.TaxiDriverClient.StartJob(Id, activeJob.Value.PickUpLocation);
        }

        public void RiderReady()
        {
            if (activeJob == null)
            {
                logger.Warn($"Player {Id} received a Rider Ready notification but no job is active!");
                return;
            }

            var rider = CyberpunkSdk.Server.PlayerSystem.GetById(activeJob.Value.PlayerId);
            if (rider == null)
            {
                logger.Warn($"Player {Id} received a Rider Ready notification but no job is active!");

                CancelJob();

                return;
            }

            rider.SendChat("SERVER", $"Taxi has picked you up! Enjoy the ride");

            var player = Server.PlayerSystem.GetById(Id);
            player.SendChat("SERVER", $"Rider is ready! Let's go.");

            isRiderReady = true;

            Cyberpunk.Rpc.Client.Plugins.TaxiDriverClient.StartTrip(Id, activeJob.Value.Location);
        }

        public void LoadJobs()
        {
            List<TaxiJobInfo> entries = new List<TaxiJobInfo>();

            foreach (var job in TaxiServer.AvailableJobs)
            {
                var data = new TaxiJobInfo();
                data.id = job.Key;
                data.pickupLocation = job.Value.PickUpLocation;
                data.reward = job.Value.Reward;

                entries.Add(data);
            }

            Cyberpunk.Rpc.Client.Plugins.TaxiDriverClient.LoadJobs(Id, entries.ToArray());
        }
    }
}
