module CyberpunkMP.Plugins

import CyberpunkMP.*
import CyberpunkMP.World.*

public native class EmoteServer extends ServerRpc {
    public static native func TriggerEmote(name: String) -> Void;
}

public class EmoteClient extends ClientRpc {
    public func TriggerEmote(serverId: Uint64, name: String) -> Void{
        let entityId = GameInstance.GetNetworkWorldSystem().GetEntityIdByServerId(serverId);
        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().HandleEmote(entityId, name);
    }
}
