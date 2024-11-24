using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace CyberpunkSdk.Game
{
    public abstract class World
    {
        public delegate void UpdateDelegate(float Delta);

        public event UpdateDelegate? UpdateEvent;

        protected void OnUpdate(float Delta)
        {
            UpdateEvent?.Invoke(Delta);
        }
    }
}
