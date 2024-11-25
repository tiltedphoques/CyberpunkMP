using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CyberpunkSdk.Utils
{
    public class CRC32
    {
        private static uint[] table;

        static CRC32()
        {
            table = new uint[256];
            for (uint i = 0; i < 256; i++)
            {
                uint crc = i;
                for (int j = 0; j < 8; j++)
                    crc = (crc & 1) == 1 ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
                table[i] = crc;
            }
        }

        public static uint Compute(string input)
        {
            byte[] bytes = Encoding.ASCII.GetBytes(input);
            uint crc = 0xFFFFFFFF;
            for (int i = 0; i < bytes.Length; i++)
            {
                byte index = (byte)(((crc) & 0xFF) ^ bytes[i]);
                crc = (crc >> 8) ^ table[index];
            }
            return ~crc;
        }
    }
}
