using Cyberpunk.Rpc.Plugins;
using CyberpunkSdk.Types;
using CyberpunkSdk.Systems;
using CyberpunkSdk;
using JobSystem;
using CyberpunkSdk.Game;


namespace Cyberpunk.Rpc.Server.Plugins
{
    public static partial class DeliveryServer
    {
        public static partial void BuyItem_Impl(ulong playerId, TweakDBID id, uint count, uint fees)
        {

        }

        public static partial void AcceptDelivery_Impl(ulong playerId, uint id)
        {
            var jobManager = Plugin.Instance.JobManager;
            if (jobManager.Get(playerId, out Job? job) && job is DeliveryDriver)
            {
                var delivery = job as DeliveryDriver;
                delivery!.AcceptJob(id);
            }
        }

        public static partial void CancelDelivery_Impl(ulong playerId)
        {
            var jobManager = Plugin.Instance.JobManager;
            if(jobManager.Get(playerId, out Job? job) && job is DeliveryDriver)
            {
                var delivery = job as DeliveryDriver;
                delivery!.CancelJob();
            }
        }

        public static partial void LoadDeliveries_Impl(ulong playerId)
        {
            var jobManager = Plugin.Instance.JobManager;
            if (jobManager.Get(playerId, out Job? job) && job is DeliveryDriver)
            {
                var delivery = job as DeliveryDriver;
                delivery!.LoadDeliveries();
            }
        }

        public static partial void LoadItems_Impl(ulong playerId)
        {
            
        }
    }
}

namespace JobSystem
{
    public struct DeliveryJob
    {
        public uint Id;
        public Cyberpunk.Rpc.Vector4 Location;
        public TweakDBID Item;
        public uint Count;
        public uint Fees;
    }

    internal class DeliverySystem
    {
        private Dictionary<uint, DeliveryJob> availableJobs = new Dictionary<uint, DeliveryJob>();
        public static DeliverySystem Instance { get; } = new DeliverySystem();
        public Dictionary<uint, DeliveryJob> AvailableJobs { get {  return availableJobs; } }

        private uint nextId = 1;

        public DeliverySystem() 
        {
            for (uint i = 0; i < 5; ++i)
            {
                var job = new DeliveryJob();

                job.Location = new Cyberpunk.Rpc.Vector4();

                job.Location.X = -807.0f;
                job.Location.Y = 833.5f;
                job.Location.Z = 26.7f;
                job.Location.W = 1.0f;

                job.Item = new TweakDBID("Items.q115_pistol");
                job.Count = 1;
                job.Fees = 100 * i;

                MakeJobAvailable(job);
            }
        }

        private uint GetJobId()
        {
            return nextId++;
        }

        internal DeliveryJob? AssignJob(uint id)
        {
            if(availableJobs.TryGetValue(id, out var job))
            {
                availableJobs.Remove(id);
                return job;
            }

            return null;
        }

        internal void MakeJobAvailable(DeliveryJob job)
        {
            job.Id = GetJobId();
            availableJobs.Add(job.Id, job);
        }
    }

    internal class DeliveryDriver : Job
    {
        private DeliveryJob? activeJob = null;

        public static readonly CName Job = new("Delivery Driver");
        public override CName Name { get { return Job; } }

        private static Logger logger = new Logger("Delivery");

        public DeliveryDriver(ulong id) : base(id)
        {
        }

        ~DeliveryDriver()
        {
            CancelJob();
        }

        public void CancelJob()
        {
            var job = activeJob;
            if (job != null)
            {
                Cyberpunk.Rpc.Client.Plugins.DeliveryClient.EndDelivery(Id, false, 0);

                DeliverySystem.Instance.MakeJobAvailable(job.Value);

                activeJob = null;

                var player = Server.PlayerSystem.GetById(Id);
                player.SendChat("SERVER", $"Delivery cancelled");
            }
        }

        public void AcceptJob(uint id)
        {

            if (activeJob != null)
            {
                logger.Warn($"Attempt from player {Id} to accept job while job is active!");
                return;
            }

            var job = DeliverySystem.Instance.AssignJob(id);
            if(job == null)
            {
                logger.Warn($"Attempt from player {Id} to accept job already accepted!");
                return;
            }

            activeJob = job;
            
            var player = Server.PlayerSystem.GetById(Id);
            player.SendChat("SERVER", $"Delivery accepted!");

            Cyberpunk.Rpc.Client.Plugins.DeliveryClient.StartDelivery(Id, activeJob.Value.Location);
        }

        public void LoadDeliveries()
        {
            List<DeliveryInfo> deliveryEntries = new List<DeliveryInfo>();

            foreach (var job in DeliverySystem.Instance.AvailableJobs)
            {
                var delivery = new DeliveryInfo();
                delivery.id = job.Value.Id;
                delivery.item = job.Value.Item;
                delivery.count = job.Value.Count;
                delivery.reward = job.Value.Fees;

                deliveryEntries.Add(delivery);
            }

            Cyberpunk.Rpc.Client.Plugins.DeliveryClient.LoadDeliveries(Id, deliveryEntries.ToArray());
        }

        public override void Update(float delta)
        {
            
        }

        public override void HeavyUpdate(float delta)
        {
            if (activeJob != null)
            {
                var player = Server.PlayerSystem.GetById(Id);
                var pos = player.MovementComponent.Position;
                var target = activeJob.Value.Location;

                var distance = System.Numerics.Vector3.DistanceSquared(new System.Numerics.Vector3(pos.X, pos.Y, pos.Z), new System.Numerics.Vector3(target.X, target.Y, target.Z));

                if (distance < 500.0f)
                {
                    Cyberpunk.Rpc.Client.Plugins.DeliveryClient.EndDelivery(Id, true, activeJob.Value.Fees);

                    player.SendChat("SERVER", $"Delivery complete! You earned {activeJob.Value.Fees}");

                    activeJob = null;
                }
            }
        }
    }
}
