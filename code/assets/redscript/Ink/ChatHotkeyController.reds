module CyberpunkMP.Ink

// based on PhoneHotkeyController
public class ChatHotkeyController extends GenericHotkeyController {

  private let mainIcon: inkImageRef;
  private let questIcon: inkImageRef;
  private let callIcon: inkImageRef;

  private let messageCounterLabel: inkWidgetRef;
  private let messageCounterLabelCircle: inkWidgetRef;
  private let messageCounter: inkTextRef;
  private let messageCounterCircle: inkTextRef;

  private let journalManager: wref<JournalManager>;

  private let phoneIconAtlas: String = "base\\gameplay\\gui\\common\\icons\\atlas_common.inkatlas";
  private let phoneIconName: CName = n"ico_phone";

  private let m_proxy: ref<inkAnimProxy>;
  private let m_questImportantAnimProxy: ref<inkAnimProxy>;

  private let m_comDeviceBB: wref<IBlackboard>;
  private let m_quickSlotBB: wref<IBlackboard>;

  private let m_phoneEnabledBBId: ref<CallbackHandle>;
  private let m_isVehiclesPopupVisibleBBId: ref<CallbackHandle>;
  private let m_isRadioPopupVisibleBBId: ref<CallbackHandle>;
  private let m_isMultiplayerUIVisibleBBId: ref<CallbackHandle>;
  private let m_isChatInputVisibleBBId: ref<CallbackHandle>;
  private let m_isConnectedToServerBBId: ref<CallbackHandle>;
  private let m_isRadialMenuVisibleBBId: ref<CallbackHandle>;
  private let m_onScannerModeChangedCallback: ref<CallbackHandle>;
  private let m_contactsActiveListener: ref<CallbackHandle>;

  protected func Initialize() -> Bool {
    super.Initialize();
    this.journalManager = GameInstance.GetJournalManager(this.GetPlayer().GetGame());
    if !IsDefined(this.journalManager) {
      return false;
    };
    this.journalManager.RegisterScriptCallback(this, n"OnJournalUpdate", gameJournalListenerType.State);
    this.journalManager.RegisterScriptCallback(this, n"OnJournalUpdateVisited", gameJournalListenerType.Visited);
    this.journalManager.RegisterScriptCallback(this, n"OnTrackedEntryChanges", gameJournalListenerType.Tracked);
    if this.isNewPhoneEnabled {
      this.GetRootWidget().RegisterToCallback(n"OnPhoneDeviceSlot", this, n"OnPhoneDeviceSlot");
      this.GetRootWidget().RegisterToCallback(n"OnPhoneDeviceReset", this, n"OnPhoneDeviceReset");
    };
    this.m_quickSlotBB = this.GetBlackboardSystem().Get(GetAllBlackboardDefs().UI_QuickSlotsData);
    this.m_comDeviceBB = this.GetBlackboardSystem().Get(GetAllBlackboardDefs().UI_ComDevice);
    this.m_phoneEnabledBBId = this.m_comDeviceBB.RegisterListenerBool(GetAllBlackboardDefs().UI_ComDevice.PhoneEnabled, this, n"OnPhoneEnabledChanged");
    this.m_isVehiclesPopupVisibleBBId = this.GetUIBlackboard().RegisterListenerBool(GetAllBlackboardDefs().UIGameData.Popup_VehiclesManager_IsShown, this, n"OnVehiclesManagerPopupIsShown");
    this.m_isRadioPopupVisibleBBId = this.GetUIBlackboard().RegisterListenerBool(GetAllBlackboardDefs().UIGameData.Popup_Radio_IsShown, this, n"OnRadioManagerPopupIsShown");
    this.m_isMultiplayerUIVisibleBBId = this.GetUIBlackboard().RegisterListenerBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, this, n"OnUIMultiplayerIsShown");
    this.m_isChatInputVisibleBBId = this.GetUIBlackboard().RegisterListenerBool(GetAllBlackboardDefs().UIGameData.UIChatInputContextRequest, this, n"OnUIChatInputIsShown");
    this.m_isConnectedToServerBBId = this.GetUIBlackboard().RegisterListenerBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerConnectedToServer, this, n"OnConnectedToServer");
    this.m_isRadialMenuVisibleBBId = this.m_quickSlotBB.RegisterListenerBool(GetAllBlackboardDefs().UI_QuickSlotsData.UIRadialContextRequest, this, n"OnRadialMenuShown", true);
    this.m_contactsActiveListener = this.m_comDeviceBB.RegisterListenerBool(GetAllBlackboardDefs().UI_ComDevice.ContactsActive, this, n"OnContactsActiveChange");

    let blackboardUI = this.GetBlackboardSystem().Get(GetAllBlackboardDefs().UI_Scanner);
    this.m_onScannerModeChangedCallback = blackboardUI.RegisterListenerVariant(GetAllBlackboardDefs().UI_Scanner.ScannerMode, this, n"OnScannerModeChanged");

    if this.IsPhoneInUse() {
      this.OnPhoneDeviceSlot(null);
    };
    this.UpdateData();

    this.OnConnectedToServer(false);

    return true;
  }

  protected cb func OnPlayerAttach(player: ref<GameObject>) -> Bool {
    // let isControllingDevice: Bool;
    // let playerStateMachineBlackboard: ref<IBlackboard>;
    super.OnPlayerAttach(player);
    // playerStateMachineBlackboard = GameInstance.GetBlackboardSystem(player.GetGame()).GetLocalInstanced(GameInstance.GetPlayerSystem(player.GetGame()).GetLocalPlayerControlledGameObject().GetEntityID(), GetAllBlackboardDefs().PlayerStateMachine);
    // isControllingDevice = playerStateMachineBlackboard.GetBool(GetAllBlackboardDefs().PlayerStateMachine.IsControllingDevice);
    // this.ToggleVisibility(!isControllingDevice, true);
  }

  private final func OnVehiclesManagerPopupIsShown(value: Bool) -> Void {
    this.ToggleVisibility(!value, false);
  }

  private final func OnRadioManagerPopupIsShown(value: Bool) -> Void {
    this.ToggleVisibility(!value, false);
  }

  private final func OnRadialMenuShown(value: Bool) -> Void {
    this.ToggleVisibility(!value, false);
  }

  private final func OnUIMultiplayerIsShown(value: Bool) -> Void {
    this.ToggleVisibility(!value, false);
  }

  private final func OnContactsActiveChange(value: Bool) -> Void { 
    this.ToggleVisibility(!value, false);
  }

  private final func OnScannerModeChanged(value: Variant) -> Void {
    let evt: ref<ScanningModeEvent> = FromVariant<ref<ScanningModeEvent>>(value);
    this.ToggleVisibility(Equals(evt.mode, gameScanningMode.Inactive), false);
  }

  private final func OnUIChatInputIsShown(value: Bool) -> Void {
    // this.ToggleVisibility(!value, false);
    this.GetRootWidget().SetVisible(!value);
  }

  private final func OnConnectedToServer(value: Bool) -> Void {
    if (value) {
      this.m_buttonHintController.SetInputAction(n"UIEnterChatMessage");
      this.m_buttonHintController.SetHoldIndicatorType(inkInputHintHoldIndicationType.Press);
    } else {
      this.m_buttonHintController.SetInputAction(n"UIConnectToServer");
      this.m_buttonHintController.SetHoldIndicatorType(inkInputHintHoldIndicationType.Hold);
    }
  }

  private final func IsPhoneInUse() -> Bool {
    let activePhoneElements: Uint32;
    let phoneCallInfo: PhoneCallInformation = FromVariant<PhoneCallInformation>(this.m_comDeviceBB.GetVariant(GetAllBlackboardDefs().UI_ComDevice.PhoneCallInformation));
    if phoneCallInfo.isAudioCall && Equals(phoneCallInfo.callPhase, questPhoneCallPhase.StartCall) && Equals(phoneCallInfo.visuals, questPhoneCallVisuals.Default) {
      return true;
    };
    activePhoneElements = this.m_comDeviceBB.GetUint(GetAllBlackboardDefs().UI_ComDevice.ActivatePhoneElements);
    if Cast<Bool>(activePhoneElements) {
      return true;
    };
    return false;
  }

  protected func InitializeStatusListener() -> Void {

  }

  protected func Uninitialize() -> Void {
    super.Uninitialize();
    if IsDefined(this.journalManager) {
      this.journalManager.UnregisterScriptCallback(this, n"OnJournalUpdate");
      this.journalManager.UnregisterScriptCallback(this, n"OnJournalUpdateVisited");
      this.journalManager.UnregisterScriptCallback(this, n"OnTrackedEntryChanges");
      this.journalManager = null;
    };
    if this.isNewPhoneEnabled {
      this.GetRootWidget().UnregisterFromCallback(n"OnPhoneDeviceSlot", this, n"OnPhoneDeviceSlot");
      this.GetRootWidget().UnregisterFromCallback(n"OnPhoneDeviceReset", this, n"OnPhoneDeviceReset");
    };
    this.m_comDeviceBB.UnregisterListenerBool(GetAllBlackboardDefs().UI_ComDevice.PhoneEnabled, this.m_phoneEnabledBBId);
    this.m_comDeviceBB.UnregisterListenerBool(GetAllBlackboardDefs().UI_ComDevice.ContactsActive, this.m_contactsActiveListener);
    this.m_quickSlotBB.UnregisterListenerBool(GetAllBlackboardDefs().UI_QuickSlotsData.UIRadialContextRequest, this.m_isRadialMenuVisibleBBId);
    this.GetUIBlackboard().UnregisterListenerBool(GetAllBlackboardDefs().UIGameData.Popup_VehiclesManager_IsShown, this.m_isVehiclesPopupVisibleBBId);
    this.GetUIBlackboard().UnregisterListenerBool(GetAllBlackboardDefs().UIGameData.Popup_Radio_IsShown, this.m_isRadioPopupVisibleBBId);
    this.GetUIBlackboard().UnregisterListenerBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, this.m_isMultiplayerUIVisibleBBId);
    this.GetBlackboardSystem().Get(GetAllBlackboardDefs().UI_Scanner).UnregisterListenerVariant(GetAllBlackboardDefs().UI_Scanner.ScannerMode, this.m_onScannerModeChangedCallback);
  }

  // crashes??
  // protected func IsInDefaultState() -> Bool {
  //   return true;
  // }

  protected func ResolveState() -> Void {
    this.GetRootWidget().SetState(n"Default");
  }

  private final func OnPhoneEnabledChanged(val: Bool) -> Void {
    this.ResolveState();
  }

  protected cb func OnPhoneDeviceSlot(target: wref<inkWidget>) -> Bool {
    if IsDefined(this.m_proxy) {
      this.m_proxy.Stop();
      this.m_proxy = null;
    };
    this.m_proxy = this.PlayLibraryAnimation(n"phone_device_slot");
  }

  protected cb func OnPhoneDeviceReset(target: wref<inkWidget>) -> Bool {
    if IsDefined(this.m_proxy) {
      this.m_proxy.Stop();
      this.m_proxy = null;
    };
    this.m_proxy = this.PlayLibraryAnimation(n"phone_device_reset");
  }

  private final func UpdateData() -> Void {
    inkWidgetRef.SetVisible(this.messageCounterLabel, false);
    inkWidgetRef.SetVisible(this.messageCounterLabelCircle, false);
    inkWidgetRef.SetVisible(this.questIcon, false);
    this.ResolveState();
  }

  private final func CountMessages(convEntry: wref<JournalPhoneConversation>, tracked: script_ref<array<Int32>>, out questImportantMessages: Int32, out unreadMessages: Int32) -> Void {
    let choiceEntry: wref<JournalPhoneChoiceEntry>;
    let hasQuestRelatedMsg: Bool;
    let k: Int32;
    let messages: array<wref<JournalEntry>>;
    let msgEntry: wref<JournalEntry>;
    let replies: array<wref<JournalEntry>>;
    this.journalManager.GetMessagesAndChoices(convEntry, messages, replies);
    hasQuestRelatedMsg = MessengerUtils.ContainsQuestRelatedMessage(this.journalManager, tracked, messages);
    if hasQuestRelatedMsg {
      questImportantMessages += 1;
      return;
    };
    k = 0;
    while k < ArraySize(replies) {
      choiceEntry = replies[k] as JournalPhoneChoiceEntry;
      if choiceEntry.IsQuestImportant() {
        questImportantMessages += 1;
        return;
      };
      k += 1;
    };
    k = 0;
    while k < ArraySize(messages) {
      msgEntry = messages[k];
      if IsDefined(msgEntry) && !this.journalManager.IsEntryVisited(msgEntry) {
        unreadMessages += 1;
        return;
      };
      k += 1;
    };
    k = 0;
    while k < ArraySize(replies) {
      choiceEntry = replies[k] as JournalPhoneChoiceEntry;
      if !choiceEntry.IsQuestImportant() {
        unreadMessages += 1;
        return;
      };
      k += 1;
    };
  }

  private final func QuestImportantBlink(enable: Bool) -> Void {
    let animOptions: inkAnimOptions;
    if enable {
      if !this.m_questImportantAnimProxy.IsPlaying() {
        animOptions.loopInfinite = true;
        animOptions.loopType = inkanimLoopType.Cycle;
        this.m_questImportantAnimProxy = this.PlayLibraryAnimation(n"quest_important", animOptions);
      };
    } else {
      this.m_questImportantAnimProxy.GotoEndAndStop();
      this.m_questImportantAnimProxy = null;
    };
  }

  protected cb func OnJournalUpdate(entryHash: Uint32, className: CName, notifyOption: JournalNotifyOption, changeType: JournalChangeType) -> Bool {
    if Equals(className, n"gameJournalPhoneMessage") || Equals(className, n"gameJournalPhoneChoiceEntry") || Equals(className, n"gameJournalQuestCodexLink") {
      this.UpdateData();
    };
  }

  protected cb func OnJournalUpdateVisited(entryHash: Uint32, className: CName, notifyOption: JournalNotifyOption, changeType: JournalChangeType) -> Bool {
    if Equals(className, n"gameJournalPhoneMessage") {
      this.UpdateData();
    };
  }

  protected cb func OnTrackedEntryChanges(hash: Uint32, className: CName, notifyOption: JournalNotifyOption, changeType: JournalChangeType) -> Bool {
    this.UpdateData();
  }

  protected func IsInDefaultState() -> Bool {
    let phoneSystem: wref<PhoneSystem> = GameInstance.GetScriptableSystemsContainer(this.GetPlayer().GetGame()).Get(n"PhoneSystem") as PhoneSystem;
    let phoneEnabled: Bool = phoneSystem.IsPhoneEnabled();
    return phoneEnabled;
  }

  private final func RestoreDefaultIcon() -> Void {
    inkWidgetRef.SetVisible(this.mainIcon, true);
  }

  protected cb func OnDpadActionPerformed(evt: ref<DPADActionPerformed>) -> Bool {
    let animName: CName;
    if Equals(this.m_hotkey, evt.action) {
      if !this.IsInDefaultState() {
        // animName = StringToName("onFailUse_" + EnumValueToString("EHotkey", EnumInt(this.m_hotkey)));
        animName = n"onFailUse_DPAD_DOWN";
        this.PlayLibraryAnimation(animName);
        return false;
      };
      if Equals(evt.state, EUIActionState.COMPLETED) && evt.successful {
        // animName = StringToName("onUse_" + EnumValueToString("EHotkey", EnumInt(this.m_hotkey)));
        animName = n"onUse_DPAD_DOWN";
        this.PlayLibraryAnimation(animName);
      } else {
        if !evt.successful {
        //   animName = StringToName("onFailUse_" + EnumValueToString("EHotkey", EnumInt(this.m_hotkey)));
            animName = n"onFailUse_DPAD_DOWN";
          this.PlayLibraryAnimation(animName);
        };
      };
    };
  }
}