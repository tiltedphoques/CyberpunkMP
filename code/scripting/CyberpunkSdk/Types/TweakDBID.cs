namespace CyberpunkSdk.Types
{
    public class TweakDBID
    {
        public uint Hash { get; private set; }
        public byte Length { get; private set; }

        public TweakDBID(uint hash, byte length) 
        {
            Hash = hash;
            Length = length;
        }

        public TweakDBID(string name)
        {
            Hash = Utils.CRC32.Compute(name);
            Length = (byte)name.Length;
        }

        public override int GetHashCode()
        {
            return Hash.GetHashCode();
        }

        public override bool Equals(object? obj)
        {
            return Equals((TweakDBID?)obj);
        }

        public bool Equals(TweakDBID? other)
        {
            return other != null && Hash == other.Hash;
        }
    }
}
