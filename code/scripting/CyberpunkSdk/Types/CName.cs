namespace CyberpunkSdk.Types
{
    public class CName
    {
        private static readonly Dictionary<ulong, string> _values = new Dictionary<ulong, string>();

        static CName()
        {
            _values[0] = "None";
        }

        private ulong FNV1a(byte[] bytes)
        {
            const ulong fnv64Offset = 0xCBF29CE484222325;
            const ulong fnv64Prime = 0x100000001b3;
            ulong hash = fnv64Offset;

            for (var i = 0; i < bytes.Length; i++)
            {
                hash = hash ^ bytes[i];
                hash *= fnv64Prime;
            }

            return hash;
        }

        public ulong Hash { get; private set; }

        public CName(string value)
        {
            if(value == "None")
            {
                Hash = 0;
                return;
            }

            var data = System.Text.Encoding.ASCII.GetBytes(value);
            Hash = FNV1a(data);

            lock(_values)
            {
                _values[Hash] = value;
            }
        }

        public CName(ulong hash) 
        {
            Hash = hash;
        }

        public override int GetHashCode()
        {
            return Hash.GetHashCode();
        }

        public override bool Equals(object? obj)
        {
            return Equals(obj as CName);
        }

        public bool Equals(CName? other)
        {
            return other != null && Hash == other.Hash;
        }

        public override string ToString()
        {
            lock(_values)
            {
                if (_values.TryGetValue(Hash, out var value))
                    return value;

                return "CName:" + Hash.ToString();
            }
        }
    }
}
