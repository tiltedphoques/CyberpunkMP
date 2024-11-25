using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Server.Loader.Game
{
    public class World : CyberpunkSdk.Game.World
    {
        internal World()
        {
        }

        internal void Update(float Delta)
        {
            OnUpdate(Delta);
        }
    }
}
