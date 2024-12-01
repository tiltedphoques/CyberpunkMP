module CyberpunkMP.Ink

import CyberpunkMP.*

public class ChatMessageController extends ListItemController {
    public let m_authorWidget: inkTextRef;
    public let m_messageWidget: inkTextRef;

    public let m_data: ref<ChatMessageData>;
    public let m_authorLabel: wref<inkText>;
    public let m_messageLabel: wref<inkText>;

    protected cb func OnInitialize() -> Bool {
        LogChannel(n"DEBUG", s"[ChatMessageController] OnInitialize");
        // super.OnInitialize();
        this.m_authorLabel = inkTextRef.Get(this.m_authorWidget) as inkText;
        this.m_messageLabel = inkTextRef.Get(this.m_messageWidget) as inkText;
        // this.RegisterToCallback(n"OnAddedToList", this, n"OnAddedToList");
    }

    protected cb func OnUninitialize() -> Bool {
        // this.UnregisterFromCallback(n"OnAddedToList", this, n"OnAddedToList");
    }
    
    protected cb func OnDataChanged(value: ref<IScriptable>) -> Bool {
        LogChannel(n"DEBUG", s"[ChatMessageController] OnDataChanged");
        // super.OnDataChanged(value);
        this.m_data = value as ChatMessageData;
        if this.m_data.m_needsAuthorLabel {
            this.m_authorLabel.SetVisible(true);
            this.m_authorLabel.SetText(this.m_data.m_author);
            if this.m_data.m_isSelf {
                // this.m_authorLabel.SetState(n"Player");
                this.m_authorLabel.SetState(n"Quest");
            } else {
                this.m_authorLabel.SetState(n"Default");
            }
        } else {
            this.m_authorLabel.SetVisible(false);
        }
        this.m_messageLabel.SetText(this.m_data.m_message);
    }

    public final func Refresh(value: ref<IScriptable>) -> Void {
        // LogChannel(n"DEBUG", s"[ChatMessageController] Refresh");
        this.m_data = value as ChatMessageData;
        if this.m_data.m_needsAuthorLabel {
            this.m_authorLabel.SetVisible(true);
            this.m_authorLabel.SetText(this.m_data.m_author);
            if this.m_data.m_isSelf {
                // this.m_authorLabel.SetState(n"Player");
                this.m_authorLabel.SetState(n"Quest");
            } else {
                this.m_authorLabel.SetState(n"Default");
            }
        } else {
            this.m_authorLabel.SetVisible(false);
        }
        this.m_messageLabel.SetText(this.m_data.m_message);
    }

    protected cb func OnAddedToList(target: wref<ListItemController>) -> Bool {
        // LogChannel(n"DEBUG", s"[ChatMessageController] OnAddedToList");
        let m_animListText: ref<inkAnimDef>;
        let m_animTextInterp: ref<inkAnimTextOffset>;
        let stageOneTime: Float;
        let stageTwoTime: Float;
        if this.GetIndex() == 0 {
            m_animListText = new inkAnimDef();
            m_animTextInterp = new inkAnimTextOffset();
            m_animTextInterp.SetDuration(0.08);
            m_animTextInterp.SetStartProgress(0.25);
            m_animTextInterp.SetEndProgress(0.00);
            m_animListText.AddInterpolator(m_animTextInterp);
            this.GetRootWidget().PlayAnimation(m_animListText);
        } else {
            stageOneTime = 0.10;
            stageTwoTime = 0.10 + MinF(5.00, Cast<Float>(this.GetIndex())) * 0.15;
            m_animListText = new inkAnimDef();
            m_animTextInterp = new inkAnimTextOffset();
            m_animTextInterp.SetStartDelay(0.00);
            m_animTextInterp.SetDuration(stageOneTime);
            m_animTextInterp.SetStartProgress(0.75);
            m_animTextInterp.SetEndProgress(0.01);
            m_animTextInterp.SetType(inkanimInterpolationType.Quadratic);
            m_animTextInterp.SetMode(inkanimInterpolationMode.EasyOut);
            m_animListText.AddInterpolator(m_animTextInterp);
            m_animTextInterp = new inkAnimTextOffset();
            m_animTextInterp.SetStartDelay(stageOneTime);
            m_animTextInterp.SetDuration(stageTwoTime);
            m_animTextInterp.SetStartProgress(0.01);
            m_animTextInterp.SetEndProgress(0.00);
            m_animTextInterp.SetType(inkanimInterpolationType.Quadratic);
            m_animTextInterp.SetMode(inkanimInterpolationMode.EasyOut);
            m_animListText.AddInterpolator(m_animTextInterp);
            this.GetRootWidget().PlayAnimation(m_animListText);
        };
    }
}
