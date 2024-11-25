namespace Server.Loader.Systems
{
    internal class Statistics
    {
        public class RpcRecord
        {
            public string Name = "";
            public int CallCount;
            public bool Server;
            public ulong Bytes;
        }

        private readonly List<RpcRecord> rpcStatistics = new List<RpcRecord>();

        public IList<RpcRecord> Rpcs => rpcStatistics;

        public void AddRpcFunction(string name, bool server)
        {
            rpcStatistics.Add(new RpcRecord
            {
                Name = name,
                CallCount = 0,
                Server = server,
                Bytes = 0
            });
        }

        public void RecordCall(int opcode, int bytes)
        {
            var record = rpcStatistics[opcode];

            record.CallCount++;
            record.Bytes += (ulong)bytes;
        }
    }
}