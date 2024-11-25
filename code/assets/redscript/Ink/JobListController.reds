module CyberpunkMP.Ink

import CyberpunkMP.*
import CyberpunkMP.Plugins.*

// PhoneDialerLogicController
public class JobListController extends inkLogicController {

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

  public let m_headerText: wref<inkText>;

  public let m_scrollArea: wref<inkScrollArea>;

  public let m_scrollControllerWidget: wref<inkScrollArea>;

  public let m_acceptButtonWidget: inkWidgetRef;

  public let m_action2ButtonWidget: inkWidgetRef;

  public let m_showAllButtonWidget: inkWidgetRef;

  public let m_showAllLabel: inkTextRef;

  public let m_nothingToReadMessageWidget: inkWidgetRef;

  public let m_scrollBarWidget: inkWidgetRef;

  public let m_listController: wref<ListController>;

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

  protected cb func OnInitialize() -> Bool {
    this.GetRootWidget().SetVisible(false);
    this.m_headerText = this.GetWidget(n"container/header") as inkText;
    this.m_headerText.SetText("Select job type");
    this.m_scrollArea = this.GetWidget(n"container/scrollarea") as inkScrollArea;
    if !IsDefined(this.m_scrollArea) {
        CMPLog(s"COULD NOT FIND SCROLLAREA");
    }
    this.m_listController = this.GetWidget(n"container/scrollarea/list").GetController() as ListController;
    if !IsDefined(this.m_listController) {
        CMPLog(s"COULD NOT FIND LIST CONTROLLER");
    }
    this.m_scrollArea.RegisterToCallback(n"OnScrollChanged", this, n"OnScrollChanged");
    this.m_scrollController = this.m_scrollArea.GetControllerByType(n"inkScrollController") as inkScrollController;
    if !IsDefined(this.m_scrollController) {
        CMPLog(s"COULD NOT FIND SCROLL CONTROLLER");
    }
    // this.m_leftMargin = new inkMargin(35.00, 10.00, 0.00, 0.00);
    // this.m_rightMargin = new inkMargin(1400.00, 10.00, 0.00, 0.00);
    // this.m_pulseAnim = new PulseAnimation();
    // this.m_pulseAnim.Configure(inkWidgetRef.Get(this.m_callsQuestFlag), 1.00, 0.20, 0.60);
    
    let deliveryJob = new JobType();
    deliveryJob.m_name = "Delivery Driver";
    deliveryJob.m_description = "Courier items to customers";
    // jobType.m_test = true;
    this.m_listController.PushData(deliveryJob, true);

    let taxiJob = new JobType();
    taxiJob.m_name = "Taxi Driver";
    taxiJob.m_description = "Pickup passengers and take them to their destination";
    this.m_listController.PushData(taxiJob, true);

    let taxiRiderJob = new JobType();
    taxiRiderJob.m_name = "Taxi Rider";
    taxiRiderJob.m_description = "Request a ride to a random location";
    this.m_listController.PushData(taxiRiderJob, true);

  }

  protected cb func OnUninitialize() -> Bool {
    this.CloseList();
  }

  protected cb func OnScrollChanged(value: Vector2) -> Bool {
    this.m_scrollController.UpdateScrollPositionFromScrollArea();
  }

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

  public final func GetSelectedJobData() -> wref<JobType> {
    let index = this.m_listController.GetSelectedIndex();
    if index >= 0 {
        let widget = this.m_listController.GetItemAt(index);
        if IsDefined(widget) {
            let item: ref<JobListItemController> = widget.GetController() as JobListItemController;
            return item.GetItemData();
        } else {
            return null;
        }
    } else {
        return null;
    }
  }

  public final func CloseList() -> Void {
    this.GetRootWidget().CallCustomCallback(n"OnCloseList");
  }
}
