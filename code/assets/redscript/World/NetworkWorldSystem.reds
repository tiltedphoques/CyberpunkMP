module CyberpunkMP.World

import Codeware.*
import CyberpunkMP.*

public native class NetworkWorldSystem extends IGameSystem {
    public native func Connect(address: String) -> Void;
    public native func Disconnect() -> Void;
    public native func GetEntityIdByServerId(serverId: Uint64) -> EntityID;
    public native func GetAppearanceSystem() -> ref<AppearanceSystem>;
    public native func GetChatSystem() -> ref<ChatSystem>;
    public native func GetInterpolationSystem() -> ref<InterpolationSystem>;
    public native func GetVehicleSystem() -> ref<VehicleSystem>;

    public func OnConnected() -> Void {
        // let evt: ref<ConnectedToServer>;
        // evt.m_connected = true;
        // GameInstance.GetUISystem(GetGameInstance()).QueueEvent(evt);
        
        let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
        let blackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
        blackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerConnectedToServer, true, true);
    }

    public func OnDisconnected(reason: Uint32) -> Void {
        // let evt: ref<ConnectedToServer>;
        // evt.m_connected = true;
        // GameInstance.GetUISystem(GetGameInstance()).QueueEvent(evt);
        
        let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
        let blackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
        blackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerConnectedToServer, false, true);
    }

    public func CreatePuppet(position: Vector4, rotation: Quaternion, isMale: Bool) -> EntityID {
        let npcSpec = new DynamicEntitySpec();

        if isMale {
            npcSpec.recordID = t"Character.MaMuppet";
        } else {
            npcSpec.recordID = t"Character.WaMuppet";
            // npcSpec.recordID = t"Character.Panam";
        }
        npcSpec.alwaysSpawned = true;
        npcSpec.position = position;
        npcSpec.orientation = rotation;
        npcSpec.persistState = false;
        npcSpec.persistSpawn = false;
        npcSpec.tags = [n"CyberpunkMP.Puppet"];


        return GameInstance.GetDynamicEntitySystem().CreateEntity(npcSpec);
    }

    public func DeletePuppet(entityId: EntityID) {
        GameInstance.GetDynamicEntitySystem().DeleteEntity(entityId);
    }
}

@addMethod(GameInstance)
public static native func GetNetworkWorldSystem() -> ref<NetworkWorldSystem>