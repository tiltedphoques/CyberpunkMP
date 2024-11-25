module CyberpunkMP.Plugins

import CyberpunkMP.*
import CyberpunkMP.World.*
import CyberpunkMP.Ink.*

public struct TaxiJobInfo {
    let id: Uint32;
    let pickupLocation: Vector4;
    let reward: Uint32;
}

public class TaxiJobEntry extends JobEntry {
    let pickupLocation: Vector4;
}

public native class TaxiServer extends ServerRpc {

    public static native func CreateJob(location: Vector4) -> Void; // Rider creates a job

    public static native func AcceptJob(id: Uint32) -> Void; // Driver accepts the job

    public static native func RiderReady() -> Void; // Rider is mounted & ready

    public static native func CancelJob() -> Void; // Rider or Driver cancels the job, only works if a job is in progress

    public static native func LoadJobs() -> Void; // Load the jobs
}

public class TaxiDriverClient extends ClientRpc {

    public let m_riderID: EntityID;
    public let m_mappin: NewMappinID;

    // Driver has accepted the job
    public func StartJob(pickupLocation: Vector4) -> Void {
        CMPLog(s"");
        
        // create mappin where the job was created
        let mappinData: MappinData;
        let mappinSystem: ref<MappinSystem> = GameInstance.GetMappinSystem(GetGameInstance());
        mappinData.mappinType = t"Mappins.QuestStaticMappinDefinition";
        mappinData.variant = gamedataMappinVariant.CPO_PingGoHereVariant;
        mappinData.active = true;
        mappinData.visibleThroughWalls = true;
        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_mappin = mappinSystem.RegisterMappin(mappinData, pickupLocation);
    }

    // Driver is close enough & is stopped, so rider can mount vehicle
    public func MountPassenger(rider_server_id: Uint64) {
        CMPLog(s"");

        // get rid of mappin

        // maybe we don't need this?
        // mount muppet in vehicle
        // this.m_riderID = GameInstance.GetNetworkWorldSystem().GetEntityIdByServerId(rider_server_id);
        // let mountData = new MountEventData();
        // mountData.slotName = n"front_right_seat";
        // mountData.mountParentEntityId =  GetPlayer(GetGameInstance()).GetMountedVehicle().GetEntityID();
        // mountData.isInstant = false;
        // mountData.ignoreHLS = true;

        // let evt = new MountAIEvent();
        // evt.name = n"Mount";
        // evt.data = mountData;

        // let entity_system = GameInstance.GetDynamicEntitySystem();
        // let passegner = entity_system.GetEntity(this.m_riderID) as GameObject;
        // passegner.QueueEvent(evt);
    }

    // Rider has mounted the vehicle
    public func StartTrip(dropoffLocation: Vector4) {
        CMPLog(s"");

        // create mappin at the destination
        let mappinData: MappinData;
        let mappinSystem: ref<MappinSystem> = GameInstance.GetMappinSystem(GetGameInstance());

        // Remove all mappin
        mappinSystem.UnregisterMappin(GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_mappin);

        mappinData.mappinType = t"Mappins.QuestStaticMappinDefinition";
        mappinData.variant = gamedataMappinVariant.CPO_PingGoHereVariant;
        mappinData.active = true;
        mappinData.visibleThroughWalls = true;
        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_mappin = mappinSystem.RegisterMappin(mappinData, dropoffLocation);

        
    }

    // User is close enough to the waypoint and is stopped, so NPC can get out
    public func EndTrip(success: Bool, reward: Uint32) -> Void {
        CMPLog(s"");

        let mappinSystem: ref<MappinSystem> = GameInstance.GetMappinSystem(GetGameInstance());
        mappinSystem.UnregisterMappin(GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_mappin);

        // Called when the Taxi is successful or failed
        if success {
            let player = GetPlayer(GetGameInstance());
            let ts = GameInstance.GetTransactionSystem(GetGameInstance());
            ts.GiveItem(player, MarketSystem.Money(), Cast<Int32>(reward));
        };

        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_controller.OnDeliveryComplete();
    }

    public func LoadJobs(jobs: array<TaxiJobInfo>) -> Void {
        CMPLog(s"");

        // Set the job list
        ArrayClear(GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_deliveryEntries);
        let i = 0;
        while i < ArraySize(jobs) {
            let entry = new TaxiJobEntry();
            entry.id = jobs[i].id;
            entry.reward = jobs[i].reward;
            // entry.pickupLocation = jobs[i].pickupLocation;
            entry.name = "Pick up a player";
            ArrayPush(GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_deliveryEntries, entry);
            i += 1;
        }
        // GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_deliveryEntries = jobs;
        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_controller.ShowDeliveryListCallback();
    }
}

public class TaxiRiderClient extends ClientRpc {
    public let m_vehicle_id: EntityID;

    // vehicle is close & slow enough to mount
    public func StartTrip(vehicle_server_id: Uint64) -> Void {
        CMPLog(s"");
    
        // mount muppet in vehicle
        // this.m_vehicle_id = GameInstance.GetNetworkWorldSystem().GetEntityIdByServerId(vehicle_server_id);
        // let mountData = new MountEventData();
        // mountData.slotName = n"front_right_seat";
        // mountData.mountParentEntityId = this.m_vehicle_id;
        // mountData.isInstant = false;
        // mountData.ignoreHLS = true;

        // let evt = new MountAIEvent();
        // evt.name = n"Mount";
        // evt.data = mountData;

        // GetPlayer(GetGameInstance()).QueueEvent(evt);
    }

    // trip is ended/cancelled
    public func EndTrip(success: Bool, cost: Uint32) -> Void {
        CMPLog(s"");

        // Called when the Taxi is successful or failed
        if success {
            let player = GetPlayer(GetGameInstance());
            let ts = GameInstance.GetTransactionSystem(GetGameInstance());
            ts.RemoveItem(player, MarketSystem.Money(), Cast<Int32>(cost));
        };

        // exit vehicle
        // let exitEvent = new AIEvent();
        // exitEvent.name = n"ExitVehicle";
        // GetPlayer(GetGameInstance()).QueueEvent(exitEvent);
    }
}