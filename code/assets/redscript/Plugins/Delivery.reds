module CyberpunkMP.Plugins

import CyberpunkMP.*
import CyberpunkMP.World.*
import CyberpunkMP.Ink.*

public struct ShopEntry {
    let item: TweakDBID;
    let count: Uint32;
    let description: String;
}

public class DeliveryEntry extends JobEntry {
    let item: TweakDBID;
    let count: Uint32;
}

public struct DeliveryInfo {
    let id: Uint32;
    let item: TweakDBID;
    let count: Uint32;
    let reward: Uint32;
}

public native class DeliveryServer extends ServerRpc {
    public static native func BuyItem(id: TweakDBID, count: Uint32, fees: Uint32) -> Void; // Shop goer calls this function to buy an item, also sets the fees (aka reward for delivery) 
    
    public static native func AcceptDelivery(id: Uint32) -> Void; // Delivery person accepts the delivery
       
    public static native func CancelDelivery() -> Void; // Delivery person cancels the delivery, only works if a delivery is in progress

    public static native func LoadItems() -> Void; // Load the items in the shop

    public static native func LoadDeliveries() -> Void; // Load the deliveries
}

public class DeliveryClient extends ClientRpc {

    public func ReceiveDelivery(item: TweakDBID, count: Uint32) -> Void {
        // Add to local player's inventory
    }

    public func StartDelivery(location: Vector4) -> Void {
        CMPLog(s"");
        // Set the waypoint, show some UI marker
        
        let mappinData: MappinData;
        let mappinSystem: ref<MappinSystem> = GameInstance.GetMappinSystem(GetGameInstance());
        mappinData.mappinType = t"Mappins.QuestStaticMappinDefinition";
        // barcode thing, ref base\\gameplay\\gui\\widgets\\mappins\\cyberspace\\cyberspace_object_mappin.inkwidget
        // mappinData.variant = gamedataMappinVariant.CyberspaceObject;
        // red phone thing, ref base\\gameplay\\gui\\widgets\\mappins\\interaction\\quick_hack_mappin.inkwidget
        // mappinData.variant = gamedataMappinVariant.PhoneCallVariant;
        // custom unused
        mappinData.variant = gamedataMappinVariant.CPO_PingGoHereVariant;
        mappinData.active = true;
        mappinData.visibleThroughWalls = true;
        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_mappin = mappinSystem.RegisterMappin(mappinData, location);
    }

    public func EndDelivery(success: Bool, reward: Uint32) -> Void {
        CMPLog(s"");
        // Called when the delivery is successful or failed
        if success {
            let player = GetPlayer(GetGameInstance());
            let ts = GameInstance.GetTransactionSystem(GetGameInstance());
            ts.GiveItem(player, MarketSystem.Money(), Cast<Int32>(reward));
        };

        let mappinSystem: ref<MappinSystem> = GameInstance.GetMappinSystem(GetGameInstance());
        mappinSystem.UnregisterMappin(GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_mappin);
        
        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_controller.OnDeliveryComplete();
    }

    public func LoadItems(items: array<ShopEntry>) -> Void {
        // Set the shop's content
    }

    public func LoadDeliveries(deliveries: array<DeliveryInfo>) -> Void {
        CMPLog(s"");

        // Set the delivery list
        ArrayClear(GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_deliveryEntries);
        let i = 0;
        while i < ArraySize(deliveries) {
            // let entry = new JobEntry();
            let entry = new DeliveryEntry();
            entry.id = deliveries[i].id;
            entry.reward = deliveries[i].reward;
            // causing crash :(
            // let item = TweakDBInterface.GetItemRecord(deliveries[i].item);
            // entry.count = deliveries[i].count;
            // if IsDefined(item) {
                // entry.name = LocKeyToString(item.DisplayName());
            // } else {
            entry.name = "Player Item";
            // }
            ArrayPush(GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_deliveryEntries, entry);
            i += 1;
        }
        // GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_deliveryEntries = deliveries;
        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_controller.ShowDeliveryListCallback();
        // let i = 0;
        // while i < ArraySize(deliveries) {
        //     CMPLog(s"deliveryID: \(deliveries[i].id)");

        //     let shop = new ShopData();
        //     shop.m_name = s"Test Shop \(deliveries[i].id)";
        //     shop.m_description = "Blah blah blah";
        //     shop.m_test = true;
        //     GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_deliveryController.m_listController.PushData(shop, false);
            
        //     i += 1;
        // }
        // GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_deliveryController.m_listController.Refresh();
    }
}
