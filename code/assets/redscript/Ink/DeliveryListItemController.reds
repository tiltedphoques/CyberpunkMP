module CyberpunkMP.Ink

import CyberpunkMP.*
import CyberpunkMP.Plugins.*

// PhoneContactItemVirtualController
public class DeliveryListItemController extends ListItemController {
    public let m_data: ref<JobEntry>;
    protected let m_root: wref<inkWidget>;
    public let m_itemName: wref<inkText>;
    public let m_itemDescription: wref<inkText>;
    public let m_itemIcon: wref<inkImage>;

    protected cb func OnInitialize() -> Bool {
        FTLog(s"[DeliveryListItemController] OnInitialize");
        // super.OnInitialize();
        this.RegisterToCallback(n"OnSelected", this, n"OnSelected");
        this.RegisterToCallback(n"OnDeselected", this, n"OnDeselected");
        this.m_root = this.GetRootWidget();
        this.m_itemName = this.GetWidget(n"textContainer/NamePreviewContainer/NameContainer/contactLabel") as inkText;
        this.m_itemDescription = this.GetWidget(n"textContainer/NamePreviewContainer/preview") as inkText;
        this.m_itemIcon = this.GetWidget(n"icon") as inkImage;
        if !IsDefined(this.m_itemName) {
            FTLog(s"[DeliveryListItemController] COULD NOT FIND LABEL");
        }
        // this.RegisterToCallback(n"OnAddedToList", this, n"OnAddedToList");
    }

    protected cb func OnUninitialize() -> Bool {
        // this.UnregisterFromCallback(n"OnAddedToList", this, n"OnAddedToList");
    }

    protected cb func OnSelected(itemController: wref<inkVirtualCompoundItemController>, discreteNav: Bool) -> Bool {
        FTLog(s"[DeliveryListItemController] OnSelected");
        // let animOptions: inkAnimOptions;
        // if IsDefined(this.m_animProxySelection) {
        //     this.m_animProxySelection.GotoStartAndStop(true);
        //     this.m_animProxySelection = null;
        // };
        // animOptions.loopInfinite = true;
        // animOptions.loopType = inkanimLoopType.Cycle;
        // this.m_animProxySelection = this.PlayLibraryAnimation(n"loopSelected", animOptions);
        this.m_root.SetState(n"Active");
        // inkWidgetRef.SetOpacity(this.m_callInputHint, this.m_isCallingEnabled ? 1.00 : 0.30);
    }

    protected cb func OnDeselected(itemController: wref<inkVirtualCompoundItemController>) -> Bool {
        // if IsDefined(this.m_animProxySelection) {
        //     this.m_animProxySelection.GotoStartAndStop(true);
        //     this.m_animProxySelection = null;
        // };
        this.m_root.SetState(n"Default");
    }

    protected cb func OnDataChanged(value: ref<IScriptable>) -> Bool {
        FTLog(s"[DeliveryListItemController] OnDataChanged");
        // super.OnDataChanged(value);
        this.m_data = value as JobEntry;
        // let itemid = ItemID.FromTDBID(this.m_data.item);
        // let item = TweakDBInterface.GetItemRecord(this.m_data.item);
        // let iconRef = new UIIconReference();
        // iconRef.iconID = item.Icon().GetID();
        // this.m_itemIcon.RequestSetImage(iconRef);
        if IsDefined(this.m_data) {
            this.m_itemName.SetText(s"\(this.m_data.name)");
            this.m_itemDescription.SetText(s"Reward: $\(this.m_data.reward)");
        }
    }

    public func GetDeliveryEntry() -> ref<JobEntry> {
        return this.m_data;
    }
}