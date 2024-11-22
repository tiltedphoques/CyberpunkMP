module CyberpunkMP.World

import Codeware.*
import CyberpunkMP.*

public native class VehicleSystem extends IScriptable {

    public native func OnVehicleEnter(entityID: EntityID, type: TweakDBID, sit_position: CName, vehicle_location: Vector4, vehicle_rotation: Quaternion) -> Void;
    public native func OnVehicleExit() -> Void;
    public native func OnVehicleReady(entityID: EntityID) -> Void;

    public func OnWorldAttached() -> Void {
        let callbackSystem = GameInstance.GetCallbackSystem();
        callbackSystem.RegisterCallback(n"Entity/Attached", this, n"OnEntityAttached")
            .AddTarget(DynamicEntityTarget.Tag(n"CyberpunkMP.Vehicle"));
    }

    // public func OnBeforeWorldDetach() -> Void {
    //     if IsDefined(this.m_callbackSystem) {
    //         this.m_callbackSystem.UnregisterCallback(n"Entity/Attached", this);
    //     }
    // }

    public func SpawnVehicle(type: TweakDBID, vehicle_location: Vector4, vehicle_rotation: Quaternion) -> EntityID {
        let entity_system = GameInstance.GetDynamicEntitySystem();

        let vehicle_spec = new DynamicEntitySpec();
        vehicle_spec.alwaysSpawned = true;
        vehicle_spec.recordID = type;
        vehicle_spec.position = vehicle_location;
        vehicle_spec.orientation = vehicle_rotation;
        vehicle_spec.persistState = false;
        vehicle_spec.persistSpawn = false;
        vehicle_spec.tags = [n"CyberpunkMP.Vehicle"];
        let vehicle_entity_id = entity_system.CreateEntity(vehicle_spec);

        return vehicle_entity_id;
    }

    private cb func OnEntityAttached(event: ref<EntityLifecycleEvent>) {
        CMPLog(s"");
        this.OnVehicleReady(event.GetEntity().GetEntityID());
    }

    public func EnterVehicle(character_id: EntityID, vehicle_id: EntityID, sit_position: CName) -> Bool {
        CMPLog(s"with seat");
        
        // works fine

        let entity_system = GameInstance.GetDynamicEntitySystem();

        // let vehicle = entity_system.GetEntity(vehicle_id) as VehicleObject;
        // if IsDefined(vehicle) {
        //     let component = vehicle.GetVehicleComponent();
        //     component.MountEntityToSlot(vehicle_id, character_id, sit_position);
        // } else {
        //     CMPLog(s"NOT DEFINED");
        // }

        // maybe better? makes them walk to the car - kinda cool

        // if !AIBehaviorScriptBase.GetAIComponent(context).GetAssignedVehicleData(vehicleID, vehicleSlotID) {
        //     this.m_result = AIbehaviorUpdateOutcome.FAILURE;
        //     return;
        // };
        // if VehicleComponent.IsSlotOccupied(ScriptExecutionContext.GetOwner(context).GetGame(), vehicleID, vehicleSlotID) {
        //     this.m_result = AIbehaviorUpdateOutcome.FAILURE;
        //     return;
        // };


        let mountData = new MountEventData();
        mountData.slotName = sit_position;
        mountData.mountParentEntityId = vehicle_id;
        mountData.isInstant = true;
        // mountData.isInstant = false;
        mountData.ignoreHLS = true;

        let evt = new MountAIEvent();
        evt.name = n"Mount";
        evt.data = mountData;

        // let entity_system = GameInstance.GetDynamicEntitySystem();
        let character = entity_system.GetEntity(character_id) as GameObject;
        character.QueueEvent(evt);



        // let evt: ref<VehicleStartedMountingEvent> = new VehicleStartedMountingEvent();

        // evt.slotID = sit_position;
        // evt.isMounting = true;
        // evt.character = character;
        // // evt.animationSlotName = this.m_exitSlot;
        // vehicle.QueueEvent(evt);
        return true;
    }

    public func ExitVehicle(character_id: EntityID) -> Bool {
        CMPLog(s"");

        let entity_system = GameInstance.GetDynamicEntitySystem();
        let character = entity_system.GetEntity(character_id) as GameObject;

        // not working
        // let mountingInfo = GameInstance.GetMountingFacility(GetGameInstance()).GetMountingInfoSingleWithObjects(character);
        // let vehicle_id = mountingInfo.parentId;
        // let vehicle = entity_system.GetEntity(vehicle_id) as VehicleObject;


        // let mountData = new MountEventData();
        // mountData.slotName = n"None";
        // mountData.mountParentEntityId = vehicle_id;
        // mountData.isInstant = true;
        // // mountData.isInstant = false;
        // mountData.ignoreHLS = true;

        // let evt = new MountAIEvent();
        // evt.name = n"Mount";
        // evt.data = mountData;

        // let entity_system = GameInstance.GetDynamicEntitySystem();
        // let character = entity_system.GetEntity(character_id);
        // character.QueueEvent(evt);

        // plays animation, but messes up orientation maybe? also crashes the game sometimes
        let exitEvent = new AIEvent();
        exitEvent.name = n"ExitVehicle";
        character.QueueEvent(exitEvent);

        // let evt: ref<VehicleStartedMountingEvent> = new VehicleStartedMountingEvent();

        // evt.slotID = vehicle.GetSlotIdForMountedObject(character);
        // evt.isMounting = false;
        // evt.character = character;
        // // evt.animationSlotName = this.m_exitSlot;
        // vehicle.QueueEvent(evt);

        return true;
    }

    // public func MountVehicle(parentID: EntityID, childId: EntityID, slot: CName) -> Void {
    //     let lowLevelMountingInfo: MountingInfo;
    //     let mountingRequest: ref<MountingRequest> = new MountingRequest();
    //     let mountData: ref<MountEventData> = new MountEventData();
    //     let mountOptions: ref<MountEventOptions> = new MountEventOptions();
    //     lowLevelMountingInfo.parentId = parentID;
    //     lowLevelMountingInfo.childId = childId;
    //     lowLevelMountingInfo.slotId.id = slot;
    //     mountingRequest.lowLevelMountingInfo = lowLevelMountingInfo;
    //     mountingRequest.preservePositionAfterMounting = true;
    //     mountingRequest.mountData = mountData;
    //     mountOptions.alive = false;
    //     mountOptions.occupiedByNonFriendly = false;
    //     mountingRequest.mountData.mountEventOptions = mountOptions;
    //     GameInstance.GetMountingFacility(GetGameInstance()).Mount(mountingRequest);
    // }
}

@wrapMethod(PlayerPuppet)
protected cb func OnMountingEvent(evt: ref<MountingEvent>) -> Bool {
    let result: Bool = wrappedMethod(evt);

    let entity_system = GameInstance.GetDynamicEntitySystem();

    let mounting_info = evt.request.lowLevelMountingInfo;
    let vehicle = entity_system.GetEntity(mounting_info.parentId) as VehicleObject;

    let type = vehicle.GetRecordID();
    let sit_position = mounting_info.slotId.id;
    let vehicle_location = vehicle.GetWorldPosition();
    let vehicle_rotation = vehicle.GetWorldOrientation();

    GameInstance.GetNetworkWorldSystem().GetVehicleSystem().OnVehicleEnter(mounting_info.parentId, type, sit_position, vehicle_location, vehicle_rotation);

    return result;
}

@wrapMethod(PlayerPuppet)
protected cb func OnUnmountingEvent(evt: ref<UnmountingEvent>) -> Bool {
    let result: Bool = wrappedMethod(evt);
    
    GameInstance.GetNetworkWorldSystem().GetVehicleSystem().OnVehicleExit();

    return result;
}