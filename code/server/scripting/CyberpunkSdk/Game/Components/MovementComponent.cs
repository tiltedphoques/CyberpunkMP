using CyberpunkSdk.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CyberpunkSdk.Game.Components
{
    public class MovementComponent
    {
        private IMovementComponent movementComponent;

        internal MovementComponent(IMovementComponent movementComponent)
        {
            this.movementComponent = movementComponent;
        }

        public Vector3 Position
        {
            get { return movementComponent.Position; }
        }

        public Vector3 Rotation
        {
            get { return movementComponent.Rotation; }
        }

        public float Velocity
        {
            get { return movementComponent.Velocity; }
        }
    }
}
