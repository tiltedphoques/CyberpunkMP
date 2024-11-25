module CyberpunkMP.Ink

import CyberpunkMP.*

// PhoneDialerLogicController
public class EmoteSelector extends inkLogicController {

  private let m_Player: wref<PlayerPuppet>;
  public let m_tabsContainer: inkWidgetRef;
  public let m_titleContainer: inkWidgetRef;
  public let m_titleTextWidget: inkTextRef;
  public let m_acceptButtonLabel: inkTextRef;
  public let m_action2ButtonLabel: inkTextRef;
  public let m_inputHintsPanel: inkWidgetRef;
  public let m_threadPanel: inkWidgetRef;
  public let m_threadList: inkWidgetRef;
  public let m_callsQuestFlag: inkWidgetRef;
  public let m_arrow: inkWidgetRef;
  public let m_threadTab: inkWidgetRef;
  public let m_unreadTab: inkWidgetRef;
  public let m_threadTabLabel: inkTextRef;
  public let m_contactsList: inkWidgetRef;
  public let m_avatarImage: inkImageRef;
  public let m_contactAvatarsFluff: inkWidgetRef;
  public let m_scrollArea: wref<inkScrollArea>;
  public let m_scrollControllerWidget: wref<inkScrollArea>;
  public let m_acceptButtonWidget: inkWidgetRef;
  public let m_action2ButtonWidget: inkWidgetRef;
  public let m_showAllButtonWidget: inkWidgetRef;
  public let m_showAllLabel: inkTextRef;
  public let m_nothingToReadMessageWidget: inkWidgetRef;
  public let m_scrollBarWidget: inkWidgetRef;
  public let m_listController: wref<ListController>;
  public let m_indicator: wref<inkWidget>;
//   public let m_dataSource: ref<ScriptableDataSource>;
//   public let m_dataView: ref<DialerContactDataView>;
//   public let m_templateClassifier: ref<DialerContactTemplateClassifier>;
  public let m_scrollController: wref<inkScrollController>;
  public let m_switchAnimProxy: ref<inkAnimProxy>;
  public let m_transitionAnimProxy: ref<inkAnimProxy>;
  public let m_horizontalMoveAnimProxy: ref<inkAnimProxy>;
  public let m_threadsController: wref<inkVirtualListController>;
//   public let m_dataSourceCache: ref<ScriptableDataSource>;
//   public let m_dataViewCache: ref<DialerContactDataView>;
  public let m_moveBehindAnimProxy: ref<inkAnimProxy>;
//   public let m_hideContactAnimProxy: ref<inkAnimProxy>;
//   public let m_contactIndexCache: Uint32;
//   public let m_menuSelectorCtrl: wref<PhoneDialerSelectionController>;
//   public let m_firstInit: Bool;
//   public let m_indexToSelect: Uint32;
//   public let m_hidingIndex: Uint32;
//   public let m_pulseAnim: ref<PulseAnimation>;
//   public let m_leftMargin: inkMargin;
//   public let m_rightMargin: inkMargin;
//   public let m_currentTab: PhoneDialerTabs;
//   public let m_callingEnabled: Bool;
  private let m_axisInputReceived: Bool = false;
  private let m_previousAngle: Float;
  private let m_previousAmount: Float;

  protected cb func OnInitialize() -> Bool {
    this.m_Player = GameInstance.GetPlayerSystem(GetGameInstance()).GetLocalPlayerMainGameObject() as PlayerPuppet;
    this.GetRootWidget().SetVisible(false);
    this.m_listController = this.GetWidget(n"container").GetController() as ListController;
    this.m_indicator = this.GetWidget(n"indicator");
    if !IsDefined(this.m_listController) {
        CMPLog(s"COULD NOT FIND LIST CONTROLLER");
    }

    // let setMenuModeEvent: ref<inkMenuLayer_SetMenuModeEvent>;
    // setMenuModeEvent = new inkMenuLayer_SetMenuModeEvent();
    // setMenuModeEvent.Init(inkMenuMode.HubMenu, inkMenuState.Enabled);
    // this.QueueBroadcastEvent(setMenuModeEvent);

    // this.m_leftMargin = new inkMargin(35.00, 10.00, 0.00, 0.00);
    // this.m_rightMargin = new inkMargin(1400.00, 10.00, 0.00, 0.00);
    // this.m_pulseAnim = new PulseAnimation();
    // this.m_pulseAnim.Configure(inkWidgetRef.Get(this.m_callsQuestFlag), 1.00, 0.20, 0.60);
    let emote = new EmoteData();
    emote.m_name = "call_police_0";
    this.m_listController.PushData(emote, false);

    emote = new EmoteData();
    emote.m_name = "walk_0";
    this.m_listController.PushData(emote, false);

    emote = new EmoteData();
    emote.m_name = "sprint_0";
    this.m_listController.PushData(emote, false);

    emote = new EmoteData();
    emote.m_name = "player__walk_0__stumble__01";
    this.m_listController.PushData(emote, false);

    emote = new EmoteData();
    emote.m_name = "add_walk_0";
    this.m_listController.PushData(emote, false);

    emote = new EmoteData();
    emote.m_name = "myAttackParried";
    this.m_listController.PushData(emote, false);

    emote = new EmoteData();
    emote.m_name = "myAttackBlocked";
    this.m_listController.PushData(emote, false);

    emote = new EmoteData();
    emote.m_name = "test";
    this.m_listController.PushData(emote, true);

    // this.RegisterToGlobalInputCallback(n"OnPostOnAxis", this, n"OnGlobalAxisInput");
    // this.RegisterToGlobalInputCallback(n"OnPostOnRelative", this, n"OnMouseInput");

    // let evt = new inkMenuLayer_SetCursorVisibility();
    // evt.Init(true, new Vector2(0.50, 0.50));
    // this.QueueEvent(evt);

    // this.SetCursorOverWidget(this.GetRootWidget());
    
    // this.m_Player.RegisterInputListener(this, n"UI_PreviousAbility");
    // this.m_Player.RegisterInputListener(this, n"UI_NextAbility");
    // this.m_Player.RegisterInputListener(this, n"UI_MoveX_Axis");
    // this.m_Player.RegisterInputListener(this, n"UI_MoveY_Axis");
    // this.m_Player.RegisterInputListener(this, n"UI_LookX_Axis");
    // this.m_Player.RegisterInputListener(this, n"UI_LookY_Axis");
  }

  protected cb func OnUninitialize() -> Bool {
    // this.m_Player.UnregisterInputListener(this, n"UI_PreviousAbility");
    // this.m_Player.UnregisterInputListener(this, n"UI_NextAbility");
    // this.m_Player.UnregisterInputListener(this, n"UI_MoveX_Axis");
    // this.m_Player.UnregisterInputListener(this, n"UI_MoveY_Axis");
    // this.m_Player.UnregisterInputListener(this, n"UI_LookX_Axis");
    // this.m_Player.UnregisterInputListener(this, n"UI_LookY_Axis");

    // let setMenuModeEvent: ref<inkMenuLayer_SetMenuModeEvent> = new inkMenuLayer_SetMenuModeEvent();
    // setMenuModeEvent.Init(inkMenuMode.HubMenu, inkMenuState.Disabled);
    // this.QueueBroadcastEvent(setMenuModeEvent);

    // let evt = new inkMenuLayer_SetCursorVisibility();
    // evt.Init(false, new Vector2(0.50, 0.50));
    // this.QueueEvent(evt);

    // this.UnregisterFromGlobalInputCallback(n"OnPostOnAxis", this, n"OnGlobalAxisInput");
    // this.UnregisterFromGlobalInputCallback(n"OnPostOnRelative", this, n"OnMouseInput");
    this.CloseEmoteSelector();
    
  }

  private final func UpdateAxisIndicator(value: Float, actionName: CName) -> Void {
    let applyChange: Bool;
    let dist: Float = -1.00;
    let amount: Float = value;
    let centerIconOldMargin: inkMargin = this.m_indicator.GetMargin();
    // if Cast<Bool>(this.m_previousAngle) {
      if AbsF(amount) > 0.00 {
        if Equals(actionName, n"mouse_x") {
          centerIconOldMargin.left += dist * -amount;
        };
        if Equals(actionName, n"mouse_y") {
          centerIconOldMargin.top += dist * amount;
        };
        // this.m_indicator.SetMargin(centerIconOldMargin);
        applyChange = true;
      };
    // } else {
    //   this.m_indicator.SetMargin(centerIconOldMargin);
    //   applyChange = true;
    // };
    // if applyChange && centerIconOldMargin.left == 0.00 && centerIconOldMargin.top == 0.00 {
    //   this.m_indicator.SetVisible(true);
    // };
    let vector = new Vector4(centerIconOldMargin.left, centerIconOldMargin.top, 0, 0);
    if Vector4.Length(vector) > 300.0 {
      let angle = (RoundF((Rad2Deg(AtanF(-vector.X, vector.Y)) + 180.0) * 8.0 / 360.0 + 0.5) + 8) % 8;
      this.m_listController.SetSelectedIndex(angle, false);
      // this.m_indicator.SetVisible(false);
      this.m_indicator.SetOpacity(0.1);
      // this.m_indicator.SetMargin(new inkMargin(0.0, 0.0, 0.0, 0.0));
    } else {
      // this.m_indicator.SetVisible(true);
      this.m_listController.SetSelectedIndex(-1, true);
      this.m_indicator.SetOpacity(1.0);
    }
    if Vector4.Length(vector) > 400.0 {
      centerIconOldMargin.left = centerIconOldMargin.left / Vector4.Length(vector) * 400.0;
      centerIconOldMargin.top = centerIconOldMargin.top / Vector4.Length(vector) * 400.0;
    }
    this.m_indicator.SetMargin(centerIconOldMargin);
    // this.m_previousAmount = amount;
  }
//   protected cb func OnScrollChanged(value: Vector2) -> Bool {
    // this.m_scrollController.UpdateScrollPositionFromScrollArea();
//   }

  public final func ShowInputHints(show: Bool) -> Void {
    inkWidgetRef.SetVisible(this.m_inputHintsPanel, show);
  }

  protected cb func OnItemSelected(previous: ref<inkVirtualCompoundItemController>, next: ref<inkVirtualCompoundItemController>) -> Bool {
    CMPLog(s"");
    // let selectionChangeEvent: ref<ContactSelectionChangedEvent>;
    // let contactData: ref<ContactData> = FromVariant<ref<IScriptable>>(next.GetData()) as ContactData;
    // InkImageUtils.RequestAvatarOrUnknown(this, this.m_avatarImage, contactData.avatarID);
    // if IsDefined(this.m_switchAnimProxy) {
    //   this.m_switchAnimProxy.Stop();
    //   this.m_switchAnimProxy = null;
    // };
    // this.m_switchAnimProxy = this.PlayLibraryAnimation(n"switchContact");
    // this.RefreshInputHints(contactData);
    // selectionChangeEvent = new ContactSelectionChangedEvent();
    // selectionChangeEvent.ContactData = contactData;
    // this.QueueEvent(selectionChangeEvent);
  }

  private final func RefreshInputHints(contactData: wref<ContactData>) -> Void {
    // if Equals(this.m_currentTab, PhoneDialerTabs.Unread) {
    //   inkWidgetRef.SetVisible(this.m_acceptButtonWidget, false);
    //   inkWidgetRef.SetVisible(this.m_action2ButtonWidget, false);
    //   inkWidgetRef.SetVisible(this.m_showAllButtonWidget, true);
    //   inkTextRef.SetLocalizedTextScript(this.m_acceptButtonLabel, "LocKey#92248");
    // } else {
    //   inkWidgetRef.SetVisible(this.m_acceptButtonWidget, contactData.isCallable);
    //   inkWidgetRef.SetVisible(this.m_action2ButtonWidget, Equals(contactData.type, MessengerContactType.Contact) ? contactData.hasMessages : true);
    //   inkWidgetRef.SetVisible(this.m_showAllButtonWidget, false);
    //   inkTextRef.SetLocalizedTextScript(this.m_acceptButtonLabel, "LocKey#22196");
    //   inkTextRef.SetLocalizedTextScript(this.m_action2ButtonLabel, "LocKey#92247");
    //   inkWidgetRef.SetVisible(this.m_acceptButtonWidget, false);
    //   inkWidgetRef.SetVisible(this.m_action2ButtonWidget, false);
    //   inkWidgetRef.SetVisible(this.m_showAllButtonWidget, false);
    // };
  }

  public final func Show() -> Void {
    this.GetRootWidget().SetVisible(true);
    // this.m_pulseAnim.Start();
    // if IsDefined(this.m_transitionAnimProxy) {
    //   this.m_transitionAnimProxy.UnregisterFromCallback(inkanimEventType.OnFinish, this, n"OnHideAnimFinished");
    //   this.m_transitionAnimProxy.Stop();
    //   this.m_transitionAnimProxy = null;
    // };
    // this.m_transitionAnimProxy = this.PlayLibraryAnimation(n"fadeIn");
  }

  public final func Hide() -> Void {
    this.GetRootWidget().SetVisible(false);
    // this.m_pulseAnim.Stop();
  }

  protected cb func OnHideAnimFinished(proxy: ref<inkAnimProxy>) -> Bool {
    this.GetRootWidget().SetVisible(false);
  }

  protected cb func OnGotFocus(evt: ref<FocusSmsMessagerEvent>) -> Bool {
    this.PlayLibraryAnimation(n"unfocus_left");
  }

  protected cb func OnLostFocus(evt: ref<UnfocusSmsMessagerEvent>) -> Bool {
    let playbackOptions: inkAnimOptions;
    playbackOptions.playReversed = true;
    this.PlayLibraryAnimation(n"unfocus_left", playbackOptions);
  }

  public final func NavigateDown() -> Void {
    this.m_listController.Next();
    this.PlaySound(n"Holocall", n"Navigation");
  }

  public final func NavigateUp() -> Void {
    this.m_listController.Prior();
    this.PlaySound(n"Holocall", n"Navigation");
  }

  public final func GetSelectedEmoteData() -> wref<EmoteData> {
    let index = this.m_listController.GetSelectedIndex();
    if index >= 0 {
        let widget = this.m_listController.GetItemAt(index);
        if IsDefined(widget) {
            let item: ref<EmoteSelectorItem> = widget.GetController() as EmoteSelectorItem;
            return item.GetEmoteData();
        } else {
            return null;
        }
    } else {
        return null;
    }
  }

  public final func CloseEmoteSelector() -> Void {
    this.GetRootWidget().CallCustomCallback(n"OnCloseEmoteSelector");
  }
}
