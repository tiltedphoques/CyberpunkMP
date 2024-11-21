@addMethod(inkTextInput)
public native func MoveCaret(position: Int32)

// UiPhone/UiVendor method

@addField(UIGameDataDef)
public let UIMultiplayerContextRequest: BlackboardID_Bool;

@addField(UIGameDataDef)
public let UIChatInputContextRequest: BlackboardID_Bool;

@addField(UIGameDataDef)
public let UIEmoteSelectionContextRequest: BlackboardID_Bool;

@addField(UIGameDataDef)
public let UIMultiplayerConnectedToServer: BlackboardID_Bool;

@addField(PlayerPuppet)
public let m_multiplayerUIActiveListener: ref<CallbackHandle>;

@addField(PlayerPuppet)
public let m_chatInputActiveListener: ref<CallbackHandle>;

@addField(PlayerPuppet)
public let m_emoteSelectionActiveListener: ref<CallbackHandle>;

@wrapMethod(PlayerPuppet)
private final func EnableUIBlackboardListener(enable: Bool) -> Void {
    wrappedMethod(enable);
    let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
    let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
    if enable {
        this.m_multiplayerUIActiveListener = uiBlackboard.RegisterListenerBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, this, n"OnUIMultiplayerContextChanged");
        this.m_chatInputActiveListener = uiBlackboard.RegisterListenerBool(GetAllBlackboardDefs().UIGameData.UIChatInputContextRequest, this, n"OnUIChatInputContextChanged");
        this.m_emoteSelectionActiveListener = uiBlackboard.RegisterListenerBool(GetAllBlackboardDefs().UIGameData.UIEmoteSelectionContextRequest, this, n"OnUIEmoteSelectionContextChanged");
    } else {
        uiBlackboard.UnregisterListenerBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, this.m_multiplayerUIActiveListener);
        this.m_multiplayerUIActiveListener = null;
        uiBlackboard.UnregisterListenerBool(GetAllBlackboardDefs().UIGameData.UIChatInputContextRequest, this.m_chatInputActiveListener);
        this.m_chatInputActiveListener = null;
        uiBlackboard.UnregisterListenerBool(GetAllBlackboardDefs().UIGameData.UIEmoteSelectionContextRequest, this.m_emoteSelectionActiveListener);
        this.m_emoteSelectionActiveListener = null;
    }
}

@addMethod(PlayerPuppet)
protected cb func OnUIMultiplayerContextChanged(value: Bool) -> Bool {
    let psmEvent: ref<PSMPostponedParameterBool>;
    if value {
        psmEvent = new PSMPostponedParameterBool();
        psmEvent.id = n"OnUIMultiplayerContextActive";
    } else {
        psmEvent = new PSMPostponedParameterBool();
        psmEvent.id = n"OnUIMultiplayerContextInactive";
    };
    psmEvent.value = true;
    this.QueueEvent(psmEvent);
}

@addMethod(PlayerPuppet)
protected cb func OnUIChatInputContextChanged(value: Bool) -> Bool {
    let psmEvent: ref<PSMPostponedParameterBool>;
    if value {
        psmEvent = new PSMPostponedParameterBool();
        psmEvent.id = n"OnUIChatInputContextActive";
    } else {
        psmEvent = new PSMPostponedParameterBool();
        psmEvent.id = n"OnUIChatInputContextInactive";
    };
    psmEvent.value = true;
    this.QueueEvent(psmEvent);
}

@addMethod(PlayerPuppet)
protected cb func OnUIEmoteSelectionContextChanged(value: Bool) -> Bool {
    let psmEvent: ref<PSMPostponedParameterBool>;
    if value {
        psmEvent = new PSMPostponedParameterBool();
        psmEvent.id = n"OnUIEmoteSelectionContextActive";
    } else {
        psmEvent = new PSMPostponedParameterBool();
        psmEvent.id = n"OnUIEmoteSelectionContextInactive";
    };
    psmEvent.value = true;
    this.QueueEvent(psmEvent);
}

public class UiMultiplayerContextDecisions extends InputContextTransitionDecisions {

  protected const func EnterCondition(const stateContext: ref<StateContext>, const scriptInterface: ref<StateGameScriptInterface>) -> Bool {
    let psmMultiplayerResult: StateResultBool = stateContext.GetTemporaryBoolParameter(n"OnUIMultiplayerContextActive");
    return psmMultiplayerResult.value;
  }

  protected const func ExitCondition(const stateContext: ref<StateContext>, const scriptInterface: ref<StateGameScriptInterface>) -> Bool {
    let psmMultiplayerResult: StateResultBool = stateContext.GetTemporaryBoolParameter(n"OnUIMultiplayerContextInactive");
    return psmMultiplayerResult.value;
  }
}

public class UiChatInputContextDecisions extends InputContextTransitionDecisions {

  protected const func EnterCondition(const stateContext: ref<StateContext>, const scriptInterface: ref<StateGameScriptInterface>) -> Bool {
    let psmChatInputResult: StateResultBool = stateContext.GetTemporaryBoolParameter(n"OnUIChatInputContextActive");
    return psmChatInputResult.value;
  }

  protected const func ExitCondition(const stateContext: ref<StateContext>, const scriptInterface: ref<StateGameScriptInterface>) -> Bool {
    let psmChatInputResult: StateResultBool = stateContext.GetTemporaryBoolParameter(n"OnUIChatInputContextInactive");
    return psmChatInputResult.value;
  }
}

public class UiEmoteSelectionContextDecisions extends InputContextTransitionDecisions {

  protected const func EnterCondition(const stateContext: ref<StateContext>, const scriptInterface: ref<StateGameScriptInterface>) -> Bool {
    let psmEmoteSelectionResult: StateResultBool = stateContext.GetTemporaryBoolParameter(n"OnUIEmoteSelectionContextActive");
    return psmEmoteSelectionResult.value;
  }

  protected const func ExitCondition(const stateContext: ref<StateContext>, const scriptInterface: ref<StateGameScriptInterface>) -> Bool {
    let psmEmoteSelectionResult: StateResultBool = stateContext.GetTemporaryBoolParameter(n"OnUIEmoteSelectionContextInactive");
    return psmEmoteSelectionResult.value;
  }
}