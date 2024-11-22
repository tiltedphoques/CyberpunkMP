module CyberpunkMP.Ink

import CyberpunkMP.*

// PhoneContactItemVirtualController
public class ServerListItemController extends ListItemController {
    public let m_data: ref<ServerData>;
    protected let m_root: wref<inkWidget>;
    public let m_serverName: wref<inkText>;
    public let m_serverDescription: wref<inkText>;

    protected cb func OnInitialize() -> Bool {
        CMPLog(s"");
        // super.OnInitialize();
        this.RegisterToCallback(n"OnSelected", this, n"OnSelected");
        this.RegisterToCallback(n"OnDeselected", this, n"OnDeselected");
        this.m_root = this.GetRootWidget();
        this.m_serverName = this.GetWidget(n"textContainer/NamePreviewContainer/NameContainer/contactLabel") as inkText;
        this.m_serverDescription = this.GetWidget(n"textContainer/NamePreviewContainer/preview") as inkText;
        if !IsDefined(this.m_serverName) {
            CMPLog(s"COULD NOT FIND LABEL");
        }
        // this.RegisterToCallback(n"OnAddedToList", this, n"OnAddedToList");
    }

    protected cb func OnUninitialize() -> Bool {
        // this.UnregisterFromCallback(n"OnAddedToList", this, n"OnAddedToList");
    }

    protected cb func OnSelected(itemController: wref<inkVirtualCompoundItemController>, discreteNav: Bool) -> Bool {
        CMPLog(s"");
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
        CMPLog(s"");
        // super.OnDataChanged(value);
        this.m_data = value as ServerData;
        this.m_serverName.SetText(this.m_data.m_name);
        this.m_serverDescription.SetText(this.m_data.m_description);
    }

    public func GetServerData() -> wref<ServerData> {
        return this.m_data;
    }
}