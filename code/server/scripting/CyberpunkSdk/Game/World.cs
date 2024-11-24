using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace CyberpunkSdk.Game
{
    public class World
    {
        public delegate void UpdateDelegate(float Delta);

        public event UpdateDelegate? UpdateEvent;

        internal World()
        {
        }

        internal void Update(float Delta)
        {
            UpdateEvent?.Invoke(Delta);
        }
    }
}
