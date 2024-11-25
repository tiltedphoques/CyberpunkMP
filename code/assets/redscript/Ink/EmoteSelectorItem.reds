module CyberpunkMP.Ink

import CyberpunkMP.*

// PhoneContactItemVirtualController
public class EmoteSelectorItem extends ListItemController {
    public let m_data: ref<EmoteData>;
    protected let m_root: wref<inkWidget>;
    public let m_name: wref<inkText>;
    public let m_container: wref<inkWidget>;
    public let m_serverDescription: wref<inkText>;

    protected cb func OnInitialize() -> Bool {
        // CMPLog(s"");
        // super.OnInitialize();
        this.RegisterToCallback(n"OnSelected", this, n"OnSelected");
        this.RegisterToCallback(n"OnDeselected", this, n"OnDeselected");
        this.RegisterToCallback(n"OnHoverOver", this, n"OnHoverOver");
        this.RegisterToCallback(n"OnHoverOut", this, n"OnHoverOut");
        this.m_root = this.GetRootWidget();
        this.m_name = this.GetWidget(n"name") as inkText;
        this.m_container = this.GetWidget(n"container") as inkWidget;
        // this.m_serverDescription = this.GetWidget(n"textContainer/NamePreviewContainer/preview") as inkText;
        if !IsDefined(this.m_name) {
            CMPLog(s"COULD NOT FIND LABEL");
        }
        this.RegisterToCallback(n"OnAddedToList", this, n"OnAddedToList");
    }

    protected cb func OnAddedToList(target: wref<ListItemController>) -> Bool {
        let offset = 400.0;
        let index = this.GetIndex();
        let widget = this.GetRootWidget();
        let margin = widget.GetMargin();
        let angle = Cast<Float>(index) * 360.0 / 8.0;
        margin.left = SinF(Deg2Rad(angle)) * offset;
        margin.top = -CosF(Deg2Rad(angle)) * offset;
        widget.SetMargin(margin);
        this.m_container.SetRotation(angle);
        this.m_container.SetScale(new Vector2(1.25, 1.25));
    }

    protected cb func OnUninitialize() -> Bool {
        this.UnregisterFromCallback(n"OnAddedToList", this, n"OnAddedToList");
    }

    protected cb func OnSelected(itemController: wref<inkVirtualCompoundItemController>, discreteNav: Bool) -> Bool {
        this.UnregisterFromCallback(n"OnHoverOver", this, n"OnHoverOver");
        this.UnregisterFromCallback(n"OnHoverOut", this, n"OnHoverOut");
        // CMPLog(s"");
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

    protected cb func OnHoverOver(e: ref<inkPointerEvent>) -> Bool {
        // let options: inkAnimOptions;
        // options.loopType = inkanimLoopType.Cycle;
        // options.loopInfinite = true;
        // this.PlayLibraryAnimation(n"pause_button_hover_over_anim");
        // this.m_animLoop = this.PlayLibraryAnimation(n"pause_button_loop_anim", options);
        this.m_root.SetState(n"Hover");
    }

    protected cb func OnHoverOut(e: ref<inkPointerEvent>) -> Bool {
        // if this.m_animLoop.IsPlaying() {
        //     this.m_animLoop.Stop();
        // };
        // this.PlayLibraryAnimation(n"pause_button_hover_out_anim");
        this.m_root.SetState(n"Default");
    }
    
    protected cb func OnDataChanged(value: ref<IScriptable>) -> Bool {
        // CMPLog(s"");
        // super.OnDataChanged(value);
        this.m_data = value as EmoteData;
        this.m_name.SetText(this.m_data.m_name);
        // this.m_serverDescription.SetText(this.m_data.m_description);
    }

    public func GetEmoteData() -> wref<EmoteData> {
        return this.m_data;
    }
}