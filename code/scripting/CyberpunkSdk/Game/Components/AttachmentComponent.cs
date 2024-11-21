using CyberpunkSdk.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CyberpunkSdk.Game.Components
{
    public class AttachmentComponent
    {
        private IAttachmentComponent attachmentComponent;

        internal AttachmentComponent(IAttachmentComponent attachmentComponent)
        {
            this.attachmentComponent = attachmentComponent;
        }

        public ulong Parent
        {
            get { return attachmentComponent.Parent; }
        }
        public ulong Slot
        {
            get { return attachmentComponent.Slot; }
        }
    }
}
