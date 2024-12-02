module CyberpunkMP.Ink

// import Codeware.UI.*
import CyberpunkMP.*
import CyberpunkMP.World.*
import CyberpunkMP.Plugins.*

// based on NewHudPhoneGameController and PauseMenuBackgroundGameController
public class MultiplayerGameController extends inkGameController {
    public let m_audioSystem: wref<AudioSystem>;
    public let m_uiSystem: wref<UISystem>;
    public let m_repeatingScrollActionEnabled: Bool = false;
    public let m_serverListLogicController: wref<ServerListController>;
    public let m_jobListLogicController: wref<JobListController>;
    public let m_deliveryListLogicController: wref<DeliveryListController>;
    public let m_emoteSelector: wref<EmoteSelector>;
    private let m_serverListWidget: wref<inkWidget>;
    private let m_jobListWidget: wref<inkWidget>;
    private let m_deliveryListWidget: wref<inkWidget>;
    private let m_emoteSelectorWidget: wref<inkWidget>;
    private let m_messageController: wref<ListController>;
    private let m_scrollArea: wref<inkScrollArea>;
    private let m_scrollController: wref<inkScrollController>;
    protected let m_player: wref<PlayerPuppet>;
    private let m_connectedToServer: Bool = false;
    private let m_serverListOpen: Bool = false;
    private let m_emoteSelectorOpen: Bool = false;
    private let m_jobListOpen: Bool = false;
    private let m_deliveryListOpen: Bool = false;
    private let m_chatInputOpen: Bool = false;
    private let m_input: wref<inkTextInput>;
    private let m_comDeviceBB: wref<IBlackboard>;
    private let m_comDeviceBBDef: ref<UI_ComDeviceDef>;
    private let m_mountedCallback: ref<CallbackHandle>;
    private let m_username: String;
    private let m_lastMessageData: ref<ChatMessageData>;
    public let m_backgroundAnimProxy: ref<inkAnimProxy>;
    private let psmBB: wref<IBlackboard>;
    private let bbListener: ref<CallbackHandle>;
    public let m_bbUiComDeviceDef: ref<UI_ComDeviceDef>;
    public let m_bbUiComDevice: wref<IBlackboard>;
    public let m_activePhoneElementsListener: ref<CallbackHandle>;
    public let m_phoneIconAnimProxy: ref<inkAnimProxy>;
    public let m_startupAnimProxy: ref<inkAnimProxy>;
    public let m_isInVehicle: Int32 = 2;
    public let m_phoneIconWidget: wref<inkWidget>;
    public let m_server: String;
    private let m_connectedToServerCallback: ref<CallbackHandle>;
    private let m_activeJob: Bool = false;
    private let m_activeDelivery: Bool = false;
    // public let m_textInput: ref<TextInput>;

    protected cb func OnInitialize() -> Bool {
        GameInstance.GetNetworkWorldSystem().GetAppearanceSystem().m_controller = this;

        FTLog(s"[MultiplayerGameController] OnInitialize");

        this.m_comDeviceBBDef = GetAllBlackboardDefs().UI_ComDevice;
        this.m_comDeviceBB = this.GetBlackboardSystem().Get(this.m_comDeviceBBDef);
        if IsDefined(this.m_comDeviceBB) {
            this.m_activePhoneElementsListener = this.m_comDeviceBB.RegisterListenerUint(this.m_comDeviceBBDef.ActivatePhoneElements, this, n"OnActivatePhoneElements", true);
        }

        let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
        let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
        let connectedToServerDef = GetAllBlackboardDefs().UIGameData.UIMultiplayerConnectedToServer;
        this.m_connectedToServerCallback = uiBlackboard.RegisterListenerBool(connectedToServerDef, this, n"OnConnectedToServer");

        // this.m_mountedCallback = this.GetBlackboardSystem().Get(GetAllBlackboardDefs().UI_ActiveVehicleData).RegisterListenerBool(GetAllBlackboardDefs().UI_ActiveVehicleData.IsPlayerMounted, this, n"OnVehicleMount", true);

        this.m_player = this.GetPlayerControlledObject() as PlayerPuppet;
        this.m_audioSystem = GameInstance.GetAudioSystem(this.m_player.GetGame());
        this.m_uiSystem = GameInstance.GetUISystem(this.m_player.GetGame());

        // this.psmBB = GameInstance.GetBlackboardSystem(this.m_player.GetGame()).Get(GetAllBlackboardDefs().PlayerStateMachine);
        // this.bbListener = this.psmBB.RegisterListenerInt(GetAllBlackboardDefs().PlayerStateMachine.Vehicle, this, n"OnPlayerEnteredVehicle", true);

        this.m_player.RegisterInputListener(this, n"UIConnectToServer");
        // this.m_player.RegisterInputListener(this, n"UIDisconnectFromServer");
        this.UpdateInputHints();

        this.m_username = "jackhumbert";
        // this.SpawnFromExternal(this.GetWidget(n"hud/wrapper"), r"mods\\cyberpunkmp\\multiplayer_ui.inkwidget", n"phone_device");
        this.AsyncSpawnFromLocal(this.GetWidget(n"hud"), n"phone_device", this, n"OnHotKeySpawn");


        let callbackSystem = GameInstance.GetCallbackSystem();
        callbackSystem.RegisterCallback(n"Entity/Attached", this, n"OnEntityAttached")
            .AddTarget(DynamicEntityTarget.Tag(n"CyberpunkMP.Puppet.Animation"));
    }

    protected cb func OnUninitialize() -> Bool {
        if IsDefined(this.m_comDeviceBB) {
            this.m_comDeviceBB.UnregisterListenerUint(this.m_comDeviceBBDef.ActivatePhoneElements, this.m_activePhoneElementsListener);
        };
        this.m_comDeviceBB = null;

        let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
        let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
        uiBlackboard.UnregisterListenerBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerConnectedToServer, this.m_connectedToServerCallback);

        // this.GetBlackboardSystem().Get(GetAllBlackboardDefs().UI_ActiveVehicleData).UnregisterListenerBool(GetAllBlackboardDefs().UI_ActiveVehicleData.IsPlayerMounted, this.m_mountedCallback);

        // if IsDefined(this.bbListener) {
        //     this.psmBB.UnregisterListenerInt(GetAllBlackboardDefs().PlayerStateMachine.Vehicle, this.bbListener);
        // };
        // this.m_player.UnregisterInputListener(this);
    }

// Hotkey

    protected cb func OnHotKeySpawn(widget: ref<inkWidget>, userData: ref<IScriptable>) -> Bool {
        this.m_phoneIconWidget = widget;
        this.m_phoneIconWidget.SetVisible(false);
        let element = this.m_comDeviceBB.GetUint(this.m_comDeviceBBDef.ActivatePhoneElements);
        this.m_isInVehicle = 0;
        if Cast<Bool>(element & Cast<Uint32>(EnumInt(gameuiActivePhoneElement.InVehicle))) {
            this.m_isInVehicle = 1;
        }
        let options: inkAnimOptions;
        options.customTimeDilation = 10.00;
        options.applyCustomTimeDilation = true;
        if this.m_isInVehicle == 1 {
            this.m_startupAnimProxy = this.PlayLibraryAnimation(n"2Vehicle", options);
        } else {
            this.m_startupAnimProxy = this.PlayLibraryAnimation(n"2Phone", options);
        }
        this.m_startupAnimProxy.RegisterToCallback(inkanimEventType.OnFinish, this, n"OnPositionAnimationFinish");
    }

    protected cb func OnPositionAnimationFinish(anim: ref<inkAnimProxy>) -> Bool {
        this.m_startupAnimProxy.UnregisterFromAllCallbacks(inkanimEventType.OnFinish);
        this.m_phoneIconWidget.SetVisible(true);
    }

    private cb func OnActivatePhoneElements(element: Uint32) -> Bool {
        FTLog(s"[MultiplayerGameController] OnActivatePhoneElements: \(element)");
        let isInVehicle = 0;
        if Cast<Bool>(element & Cast<Uint32>(EnumInt(gameuiActivePhoneElement.InVehicle))) {
            isInVehicle = 1;
        }
        if NotEquals(this.m_isInVehicle, isInVehicle) {
            this.m_isInVehicle = isInVehicle;
            if IsDefined(this.m_phoneIconAnimProxy) {
                this.m_phoneIconAnimProxy.Stop();
                this.m_phoneIconAnimProxy = null;
            };
            if this.m_isInVehicle == 1 {
                this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Vehicle");
            } else {
                this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Phone");
            }
        }
    }

/*
    protected cb func OnVehicleMount(value: Bool) -> Bool {
        FTLog(s"[MultiplayerGameController] OnPlayerEnteredVehicle");
        if value  {
            let targets = new inkWidgetsSet();
            targets.Select(this.GetWidget(n"hud"));
            this.PlayLibraryAnimationOnTargets(n"to_vehicle", targets);

            let message = new ChatMessageUIEvent();
            message.m_author = "jackhumbert";
            message.m_message = "Mounting to vehicle";
            this.QueueEvent(message);

        } else {
            let targets = new inkWidgetsSet();
            targets.Select(this.GetWidget(n"hud"));
            this.PlayLibraryAnimationOnTargets(n"from_vehicle", targets);

            let message = new ChatMessageUIEvent();
            message.m_author = "jackhumbert";
            message.m_message = "Unmounting from vehicle";
            this.QueueEvent(message);
        }
    }

    protected cb func OnPlayerEnteredVehicle(value: Int32) -> Bool {
        FTLog(s"[MultiplayerGameController] OnPlayerEnteredVehicle");
        if value != 0 {
            let targets = new inkWidgetsSet();
            targets.Select(this.GetWidget(n"hud"));
            this.PlayLibraryAnimationOnTargets(n"to_vehicle", targets);

            let message = new ChatMessageUIEvent();
            message.m_author = "jackhumbert";
            message.m_message = "Mounting to vehicle";
            this.QueueEvent(message);

        } else {
            let targets = new inkWidgetsSet();
            targets.Select(this.GetWidget(n"hud"));
            this.PlayLibraryAnimationOnTargets(n"from_vehicle", targets);

            let message = new ChatMessageUIEvent();
            message.m_author = "jackhumbert";
            message.m_message = "Unmounting from vehicle";
            this.QueueEvent(message);
        }
    }
    */

    private func UpdateInputHints() -> Void {
        let evt = new UpdateInputHintMultipleEvent();
        evt.targetHintContainer = n"GameplayInputHelper";
        // if this.m_activeDelivery {
        //     this.m_player.RegisterInputListener(this, n"UIShop");
        //     this.m_player.UnregisterInputListener(this, n"UIJob");
        // } else {
        //     this.m_player.UnregisterInputListener(this, n"UIShop");
        //     this.m_player.RegisterInputListener(this, n"UIJob");
        // }
        evt.AddInputHint(CreateInputHint(n"Connect to server", n"UIConnectToServer", true), !this.m_connectedToServer && !this.m_serverListOpen);
        evt.AddInputHint(CreateInputHint(n"Disconnect from server", n"UIDisconnectFromServer", true), this.m_connectedToServer && !this.m_serverListOpen);
        evt.AddInputHint(CreateInputHint(n"Emote", n"UIEmote", true), this.m_connectedToServer && !this.m_emoteSelectorOpen);
        evt.AddInputHint(CreateInputHint(n"Cancel Job", n"UIShop", true), this.m_connectedToServer && !this.m_deliveryListOpen && this.m_activeDelivery);
        evt.AddInputHint(CreateInputHint(n"Start Job", n"UIJob", false), this.m_connectedToServer && !this.m_deliveryListOpen && !this.m_activeDelivery);

        this.m_uiSystem.QueueEvent(evt);
    }

    private final func PlayBackgroundAnim(opt deactivation: Bool) -> Void {
        if IsDefined(this.m_backgroundAnimProxy) {
            this.m_backgroundAnimProxy.Stop();
            this.m_backgroundAnimProxy = null;
        };
        if deactivation {
            this.m_backgroundAnimProxy = this.PlayLibraryAnimation(n"bg_hide");
        } else {
            this.m_backgroundAnimProxy = this.PlayLibraryAnimation(n"bg_show");
        };
    }

    protected func OnConnectedToServer(connected: Bool) -> Void {
        FTLog(s"[MultiplayerGameController] OnConnectedToServer");
        this.m_connectedToServer = connected;
        if (connected) {
            this.ShowServerList(false);
            this.AsyncSpawnFromLocal(this.GetWidget(n"hud"), n"chat");
            this.m_player.UnregisterInputListener(this, n"UIConnectToServer");
            this.m_player.RegisterInputListener(this, n"UIDisconnectFromServer");
            this.m_player.RegisterInputListener(this, n"UIEmote");
            this.m_player.RegisterInputListener(this, n"UIJob");
        } else {
            this.GetWidget(n"hud") as inkCompoundWidget.RemoveChild(this.GetWidget(n"hud/chat"));
            this.m_player.RegisterInputListener(this, n"UIConnectToServer");
            this.m_player.UnregisterInputListener(this, n"UIDisconnectFromServer");
            this.m_player.UnregisterInputListener(this, n"UIEmote");
            this.m_player.UnregisterInputListener(this, n"UIJob");
        }
        this.UpdateInputHints();
    }

// Server List

    private final func ShowServerList(show: Bool) -> Void {
        if (show) {
            this.AsyncSpawnFromLocal(this.GetWidget(n"server_list_slot"), n"server_list", this, n"OnServerListSpawned");
            if IsDefined(this.m_phoneIconAnimProxy) {
                this.m_phoneIconAnimProxy.Stop();
                this.m_phoneIconAnimProxy = null;
            };
            this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2ServerList");
            this.PlayBackgroundAnim();
        } else {
            if IsDefined(this.m_phoneIconAnimProxy) {
                this.m_phoneIconAnimProxy.Stop();
                this.m_phoneIconAnimProxy = null;
            };
            if this.m_isInVehicle == 1 {
                this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Vehicle");
            } else {
                this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Phone");
            }
            this.OnServerListClosed();
            this.PlayBackgroundAnim(true);
        }
        this.m_serverListOpen = show;
        this.UpdateInputHints();
    }

    protected cb func OnServerListSpawned(widget: ref<inkWidget>, userData: ref<IScriptable>) -> Bool {
        if IsDefined(widget) {
            this.m_serverListWidget = widget;
            // this.m_serverListWidget.RegisterToCallback(n"OnCloseServerList", this, n"OnCloseServerList");
            this.m_serverListLogicController = widget.GetController() as ServerListController;
            if IsDefined(this.m_serverListLogicController) {
                this.m_uiSystem.PushGameContext(UIGameContext.ModalPopup);
                this.m_uiSystem.RequestNewVisualState(n"inkModalPopupState");
                TimeDilationHelper.SetTimeDilationWithProfile(this.m_player, "radialMenu", true, true);
                PopupStateUtils.SetBackgroundBlur(this, true);
                this.m_audioSystem.Play(n"ui_phone_incoming_call_positive");
                this.m_serverListLogicController.Show();
                this.PlayRumble(RumbleStrength.SuperLight, RumbleType.Slow, RumblePosition.Left);
                let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
                let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
                uiBlackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, true, true);
                this.EnableServerInput();
            } else {
                FTLog(s"[MultiplayerGameController] OnServerListSpawned - no logic controller");
            }
        } else {
            FTLog(s"[MultiplayerGameController] OnServerListSpawned - no widget");
        }
    }

    protected cb func OnServerListClosed() -> Bool {
        if IsDefined(this.m_serverListLogicController) {
            this.DisableServerInput();
            this.m_audioSystem.Play(n"ui_phone_incoming_call_negative");
            this.m_serverListLogicController.Hide();
            this.m_repeatingScrollActionEnabled = false;
            this.PlayRumble(RumbleStrength.SuperLight, RumbleType.Fast, RumblePosition.Left);
            TimeDilationHelper.SetTimeDilationWithProfile(this.m_player, "radialMenu", false, false);
            PopupStateUtils.SetBackgroundBlur(this, false);
            this.m_uiSystem.PopGameContext(UIGameContext.ModalPopup);
            this.m_uiSystem.RestorePreviousVisualState(n"inkModalPopupState");
            this.m_serverListLogicController = null;
            (this.GetWidget(n"server_list_slot") as inkCompoundWidget).RemoveChild(this.m_serverListWidget);
            this.m_serverListWidget = null;
            let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
            let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
            uiBlackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, false, true);
        };
    }

    public final func EnableServerInput() -> Void {
        this.m_player.RegisterInputListener(this, n"popup_moveDown");
        this.m_player.RegisterInputListener(this, n"popup_moveUp");
        this.m_player.RegisterInputListener(this, n"popup_moveUp_left_stick_up");
        this.m_player.RegisterInputListener(this, n"popup_moveUp_left_stick_down");
        this.m_player.RegisterInputListener(this, n"popup_goto_messenger");
        this.m_player.RegisterInputListener(this, n"OpenPauseMenu");
        this.m_player.RegisterInputListener(this, n"proceed");
        this.m_player.RegisterInputListener(this, n"secondaryAction");
        this.m_player.RegisterInputListener(this, n"cancel");
        this.m_player.RegisterInputListener(this, n"popup_prior");
        this.m_player.RegisterInputListener(this, n"popup_next");
        this.m_player.RegisterInputListener(this, n"popup_moveLeft");
        this.m_player.RegisterInputListener(this, n"popup_moveRight");
    }

    public final func DisableServerInput() -> Void {
        this.m_player.UnregisterInputListener(this, n"popup_moveDown");
        this.m_player.UnregisterInputListener(this, n"popup_moveUp");
        this.m_player.UnregisterInputListener(this, n"popup_moveUp_left_stick_up");
        this.m_player.UnregisterInputListener(this, n"popup_moveUp_left_stick_down");
        this.m_player.UnregisterInputListener(this, n"popup_goto_messenger");
        this.m_player.UnregisterInputListener(this, n"OpenPauseMenu");
        this.m_player.UnregisterInputListener(this, n"proceed");
        this.m_player.UnregisterInputListener(this, n"secondaryAction");
        this.m_player.UnregisterInputListener(this, n"cancel");
        this.m_player.UnregisterInputListener(this, n"popup_prior");
        this.m_player.UnregisterInputListener(this, n"popup_next");
        this.m_player.UnregisterInputListener(this, n"popup_moveLeft");
        this.m_player.UnregisterInputListener(this, n"popup_moveRight");
    }

    protected cb func OnCloseServerList(target: wref<inkWidget>) -> Bool {
        // this.CloseContactList();
    }

    protected cb func OnServerListAction(action: ListenerAction, consumer: ListenerActionConsumer) -> Bool {
        let actionName: CName = ListenerAction.GetName(action);
        let actionType: gameinputActionType = ListenerAction.GetType(action);
        if Equals(actionType, gameinputActionType.REPEAT) {
            if !this.m_repeatingScrollActionEnabled {
                return false;
            };
            switch actionName {
                case n"popup_moveDown":
                this.m_serverListLogicController.NavigateDown();
                break;
                case n"popup_moveUp":
                this.m_serverListLogicController.NavigateUp();
                break;
                case n"popup_moveUp_left_stick_down":
                this.m_serverListLogicController.NavigateDown();
                break;
                case n"popup_moveUp_left_stick_up":
                this.m_serverListLogicController.NavigateUp();
            };
        } else {
            if Equals(actionType, gameinputActionType.BUTTON_PRESSED) {
                if !this.m_repeatingScrollActionEnabled {
                    this.m_repeatingScrollActionEnabled = true;
                };
                switch actionName {
                case n"popup_moveDown":
                    this.m_serverListLogicController.NavigateDown();
                    break;
                case n"popup_moveUp":
                    this.m_serverListLogicController.NavigateUp();
                    break;
                case n"popup_moveUp_left_stick_down":
                    this.m_serverListLogicController.NavigateDown();
                    break;
                case n"popup_moveUp_left_stick_up":
                    this.m_serverListLogicController.NavigateUp();
                    break;
                case n"proceed":
                    // this.AcceptAction();
                    let serverData: wref<ServerData>;
                    if IsDefined(this.m_serverListLogicController) {
                        serverData = this.m_serverListLogicController.GetSelectedServerData();
                        if IsDefined(serverData) {
                            FTLog(s"[MultiplayerGameController] Server selected: \(serverData.m_name)");
                            this.m_server = serverData.m_name;
                            if serverData.m_test {
                                this.GetUIBlackboard().SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerConnectedToServer, true, true);
                            } else {
                                GameInstance.GetNetworkWorldSystem().Connect();
                            }
                        } else {
                            FTLog(s"[MultiplayerGameController] No server selected");
                        }
                    }
                    break;
                case n"OpenPauseMenu":
                    ListenerActionConsumer.DontSendReleaseEvent(consumer);
                    break;
                case n"cancel":
                    this.ShowServerList(false);
                    break;
                case n"popup_moveRight":
                case n"popup_next":
                case n"popup_prior":
                case n"popup_moveLeft":
                    // this.SelectOtherTab();
                    break;
                };
            };
        };
        return true;
    }

// Chat

    // protected cb func OnChatSpawned(widget: ref<inkWidget>, userData: ref<IScriptable>) -> Bool {
    //     FTLog(s"[MultiplayerGameController] OnChatSpawned");
    //     this.m_messageController = this.GetWidget(n"hud/chat/wrapper/chat/scrollarea/messages").GetController() as ListController;
    //     if !IsDefined(this.m_messageController) {
    //         FTLog(s"[MultiplayerGameController] OnChatSpawned - LIST CONTROLLER NOT FOUND");
    //     }
    //     this.m_scrollArea = this.GetWidget(n"hud/chat/wrapper/chat/scrollarea") as inkScrollArea;
    //     if !IsDefined(this.m_scrollArea) {
    //         FTLog(s"[MultiplayerGameController] OnChatSpawned - SCROLLAREA NOT FOUND");
    //     }
    //     // this.m_scrollArea.RegisterToCallback(n"OnScrollChanged", this, n"OnScrollChanged");
    //     // this.m_scrollController = this.GetWidget(n"hud/chat/wrapper/chat").GetController() as inkScrollController;
    //     // if !IsDefined(this.m_scrollController) {
    //         // FTLog(s"[MultiplayerGameController] OnChatSpawned - SCROLL CONTROLLER NOT FOUND");
    //     // }
    //     this.m_input = this.GetWidget(n"hud/chat/wrapper/input_box/input") as inkTextInput;
    //     if !IsDefined(this.m_input) {
    //         FTLog(s"[MultiplayerGameController] OnChatSpawned - INPUT WIDGET NOT FOUND");
    //     }

    //     this.m_messageController.Clear(true);

    //     let messageData = new ChatMessageUIEvent();
    //     messageData.m_author = "SERVER";
    //     messageData.m_message = "Connected to " + this.m_server;
    //     this.QueueEvent(messageData);

    //     if Equals(StringToName(this.m_server), n"test") {

    //         let message: ref<ChatMessageUIEvent>;

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "jackhumbert";
    //         message.m_message = "Wow, it works";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "yamashi";
    //         message.m_message = "Yeah";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "jackhumbert";
    //         message.m_message = "This is just static text though";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "yamashi";
    //         message.m_message = "Yeah";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "jackhumbert";
    //         message.m_message = "Ok, not from the server or anything";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "yamashi";
    //         message.m_message = "Yeah";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "jackhumbert";
    //         message.m_message = "Cool";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "yamashi";
    //         message.m_message = "Yeah";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "jackhumbert";
    //         message.m_message = "Neat";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "jackhumbert";
    //         message.m_message = "This is a long message to test the wrapping capabilities";
    //         this.QueueEvent(message);

    //         message = new ChatMessageUIEvent();
    //         message.m_author = "jackhumbert";
    //         message.m_message = "This is a really long message to make sure that we're actually testing the wrapping capabilities, since the box itself is currently 1000 wide";
    //         this.QueueEvent(message);
    //     }
    // }


// Emote Selector

    private final func ShowEmoteSelector(show: Bool) -> Void {
        if (show) {
            this.AsyncSpawnFromLocal(this.GetRootWidget(), n"emote_selector", this, n"OnEmoteSelectorSpawned");
            // if IsDefined(this.m_phoneIconAnimProxy) {
            //     this.m_phoneIconAnimProxy.Stop();
            //     this.m_phoneIconAnimProxy = null;
            // };
            // this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2ServerList");
            this.PlayBackgroundAnim();
        } else {
            // if IsDefined(this.m_phoneIconAnimProxy) {
            //     this.m_phoneIconAnimProxy.Stop();
            //     this.m_phoneIconAnimProxy = null;
            // };
            // if this.m_isInVehicle == 1 {
            //     this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Vehicle");
            // } else {
            //     this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Phone");
            // }
            this.OnEmoteSelectorClosed();
            this.PlayBackgroundAnim(true);
        }
        this.m_emoteSelectorOpen = show;
        this.UpdateInputHints();
    }

    protected cb func OnEmoteSelectorSpawned(widget: ref<inkWidget>, userData: ref<IScriptable>) -> Bool {
        if IsDefined(widget) {
            this.m_emoteSelectorWidget = widget;
            // this.m_emoteSelectorWidget.RegisterToCallback(n"OnCloseServerList", this, n"OnCloseServerList");
            this.m_emoteSelector = widget.GetController() as EmoteSelector;
            if IsDefined(this.m_emoteSelector) {
                this.GetSystemRequestsHandler().PauseGame();
                this.m_uiSystem.PushGameContext(UIGameContext.ModalPopup);
                this.m_uiSystem.RequestNewVisualState(n"inkModalPopupState");
                TimeDilationHelper.SetTimeDilationWithProfile(this.m_player, "radialMenu", true, true);
                PopupStateUtils.SetBackgroundBlur(this, true);

                // let cursorEvt: ref<inkMenuLayer_SetCursorType> = new inkMenuLayer_SetCursorType();
                // cursorEvt.Init(n"default", true);
                // this.QueueEvent(cursorEvt);

                this.m_audioSystem.Play(n"ui_phone_incoming_call_positive");
                this.m_emoteSelector.Show();
                this.PlayRumble(RumbleStrength.SuperLight, RumbleType.Slow, RumblePosition.Left);
                let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
                let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
                uiBlackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIEmoteSelectionContextRequest, true, true);

                this.m_player.RegisterInputListener(this, n"mouse_x");
                this.m_player.RegisterInputListener(this, n"mouse_y");

                // this.EnableServerInput();
            } else {
                FTLog(s"[MultiplayerGameController] OnEmoteSelectorSpawned - no logic controller");
            }
        } else {
            FTLog(s"[MultiplayerGameController] OnEmoteSelectorSpawned - no widget");
        }
    }

    protected cb func OnEmoteSelectorClosed() -> Bool {
        if IsDefined(this.m_emoteSelector) {
            // this.DisableServerInput();

            this.m_player.UnregisterInputListener(this, n"mouse_x");
            this.m_player.UnregisterInputListener(this, n"mouse_y");

            this.m_audioSystem.Play(n"ui_phone_incoming_call_negative");
            this.m_emoteSelector.Hide();
            this.m_repeatingScrollActionEnabled = false;
            let emoteData = this.m_emoteSelector.GetSelectedEmoteData();
            if IsDefined(emoteData) {
                FTLog(s"[MultiplayerGameController] OnEmoteSelectorClosed: \(emoteData.m_name)");
                EmoteServer.TriggerEmote(emoteData.m_name);
            } else {
                FTLog(s"[MultiplayerGameController] OnEmoteSelectorClosed - no emote selected");
            }
            this.PlayRumble(RumbleStrength.SuperLight, RumbleType.Fast, RumblePosition.Left);
            TimeDilationHelper.SetTimeDilationWithProfile(this.m_player, "radialMenu", false, false);
            PopupStateUtils.SetBackgroundBlur(this, false);
            this.m_uiSystem.PopGameContext(UIGameContext.ModalPopup);
            this.m_uiSystem.RestorePreviousVisualState(n"inkModalPopupState");
            this.GetSystemRequestsHandler().UnpauseGame();
            this.m_emoteSelector = null;
            (this.GetRootWidget() as inkCompoundWidget).RemoveChild(this.m_emoteSelectorWidget);
            this.m_emoteSelectorWidget = null;
            let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
            let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
            uiBlackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIEmoteSelectionContextRequest, false, true);
        };
    }

// Job List

    private final func ShowJobList(show: Bool) -> Void {
        if show {
            this.AsyncSpawnFromLocal(this.GetWidget(n"server_list_slot"), n"job_list", this, n"OnJobListSpawned");
            if IsDefined(this.m_phoneIconAnimProxy) {
                this.m_phoneIconAnimProxy.Stop();
                this.m_phoneIconAnimProxy = null;
            };
            this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2ServerList");
            this.PlayBackgroundAnim();
        } else {
            if IsDefined(this.m_phoneIconAnimProxy) {
                this.m_phoneIconAnimProxy.Stop();
                this.m_phoneIconAnimProxy = null;
            };
            if this.m_isInVehicle == 1 {
                this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Vehicle");
            } else {
                this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Phone");
            }
            this.OnJobListClosed();
            this.PlayBackgroundAnim(true);
        }
        this.m_jobListOpen = show;
        this.UpdateInputHints();
    }

    protected cb func OnJobListSpawned(widget: ref<inkWidget>, userData: ref<IScriptable>) -> Bool {
        if IsDefined(widget) {
            this.m_jobListWidget = widget;
            // this.m_jobListWidget.RegisterToCallback(n"OnCloseDeliveryList", this, n"OnCloseDeliveryList");
            this.m_jobListLogicController = widget.GetController() as JobListController;
            if IsDefined(this.m_jobListLogicController) {
                this.m_uiSystem.PushGameContext(UIGameContext.ModalPopup);
                this.m_uiSystem.RequestNewVisualState(n"inkModalPopupState");
                TimeDilationHelper.SetTimeDilationWithProfile(this.m_player, "radialMenu", true, true);
                PopupStateUtils.SetBackgroundBlur(this, true);
                this.m_audioSystem.Play(n"ui_phone_incoming_call_positive");
                this.m_jobListLogicController.Show();
                this.PlayRumble(RumbleStrength.SuperLight, RumbleType.Slow, RumblePosition.Left);
                let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
                let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
                uiBlackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, true, true);
                this.EnableServerInput();
            } else {
                FTLog(s"[MultiplayerGameController] OnJobListSpawned - no logic controller");
            }
        } else {
            FTLog(s"[MultiplayerGameController] OnJobListSpawned - no widget");
        }
    }

    protected cb func OnJobListClosed() -> Bool {
        if IsDefined(this.m_jobListLogicController) {
            this.DisableServerInput();
            this.m_audioSystem.Play(n"ui_phone_incoming_call_negative");
            this.m_jobListLogicController.Hide();
            this.m_repeatingScrollActionEnabled = false;
            this.PlayRumble(RumbleStrength.SuperLight, RumbleType.Fast, RumblePosition.Left);
            TimeDilationHelper.SetTimeDilationWithProfile(this.m_player, "radialMenu", false, false);
            PopupStateUtils.SetBackgroundBlur(this, false);
            this.m_uiSystem.PopGameContext(UIGameContext.ModalPopup);
            this.m_uiSystem.RestorePreviousVisualState(n"inkModalPopupState");
            this.m_jobListLogicController = null;
            (this.GetWidget(n"server_list_slot") as inkCompoundWidget).RemoveChild(this.m_jobListWidget);
            this.m_jobListWidget = null;
            let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
            let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
            uiBlackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, false, true);
        };
    }

    protected cb func OnCloseJobList(target: wref<inkWidget>) -> Bool {
        // this.CloseContactList();
    }

    protected cb func OnJobListAction(action: ListenerAction, consumer: ListenerActionConsumer) -> Bool {
        let actionName: CName = ListenerAction.GetName(action);
        let actionType: gameinputActionType = ListenerAction.GetType(action);
        if Equals(actionType, gameinputActionType.REPEAT) {
            if !this.m_repeatingScrollActionEnabled {
                return false;
            };
            switch actionName {
                case n"popup_moveDown":
                this.m_jobListLogicController.NavigateDown();
                break;
                case n"popup_moveUp":
                this.m_jobListLogicController.NavigateUp();
                break;
                case n"popup_moveUp_left_stick_down":
                this.m_jobListLogicController.NavigateDown();
                break;
                case n"popup_moveUp_left_stick_up":
                this.m_jobListLogicController.NavigateUp();
            };
        } else {
            if Equals(actionType, gameinputActionType.BUTTON_PRESSED) {
                if !this.m_repeatingScrollActionEnabled {
                    this.m_repeatingScrollActionEnabled = true;
                };
                switch actionName {
                case n"popup_moveDown":
                    this.m_jobListLogicController.NavigateDown();
                    break;
                case n"popup_moveUp":
                    this.m_jobListLogicController.NavigateUp();
                    break;
                case n"popup_moveUp_left_stick_down":
                    this.m_jobListLogicController.NavigateDown();
                    break;
                case n"popup_moveUp_left_stick_up":
                    this.m_jobListLogicController.NavigateUp();
                    break;
                case n"proceed":
                    // this.AcceptAction();
                    if IsDefined(this.m_jobListLogicController) {
                        let jobType = this.m_jobListLogicController.GetSelectedJobData();
                        // FTLog(s"[MultiplayerGameController] Job selected: \(jobType.id)");
                        // JobsServer.SelectJob(n"Delivery driver");
                        if (Equals(ToName(jobType.m_name), n"Taxi Rider")) {
                            TaxiServer.CreateJob(new Vector4(-2565.0, -5263.0, 94.25, 1.0));
                        } else {
                            JobsServer.SelectJob(ToName(jobType.m_name));
                            this.m_activeJob = true;
                        }
                        this.ShowJobList(false);
                        this.UpdateInputHints();
                    }
                    break;
                case n"OpenPauseMenu":
                    ListenerActionConsumer.DontSendReleaseEvent(consumer);
                    break;
                case n"cancel":
                    this.ShowJobList(false);
                    break;
                case n"popup_moveRight":
                case n"popup_next":
                case n"popup_prior":
                case n"popup_moveLeft":
                    // this.SelectOtherTab();
                    break;
                };
            };
        };
        return true;
    }

// Delivery List

    private final func ShowDeliveryList(show: Bool) -> Void {
        if (show) {
        } else {
            if IsDefined(this.m_phoneIconAnimProxy) {
                this.m_phoneIconAnimProxy.Stop();
                this.m_phoneIconAnimProxy = null;
            };
            if this.m_isInVehicle == 1 {
                this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Vehicle");
            } else {
                this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2Phone");
            }
            this.OnDeliveryListClosed();
            this.PlayBackgroundAnim(true);
        }
        this.m_deliveryListOpen = show;
        this.UpdateInputHints();
    }

    public final func ShowDeliveryListCallback() -> Void {
        this.AsyncSpawnFromLocal(this.GetWidget(n"server_list_slot"), n"delivery_list", this, n"OnDeliveryListSpawned");
        if IsDefined(this.m_phoneIconAnimProxy) {
            this.m_phoneIconAnimProxy.Stop();
            this.m_phoneIconAnimProxy = null;
        };
        this.m_phoneIconAnimProxy = this.PlayLibraryAnimation(n"2ServerList");
        this.PlayBackgroundAnim();
        this.m_deliveryListOpen = true;
        this.UpdateInputHints();
    }

    protected cb func OnDeliveryListSpawned(widget: ref<inkWidget>, userData: ref<IScriptable>) -> Bool {
        if IsDefined(widget) {
            this.m_deliveryListWidget = widget;
            // this.m_deliveryListWidget.RegisterToCallback(n"OnCloseDeliveryList", this, n"OnCloseDeliveryList");
            this.m_deliveryListLogicController = widget.GetController() as DeliveryListController;
            if IsDefined(this.m_deliveryListLogicController) {
                this.m_uiSystem.PushGameContext(UIGameContext.ModalPopup);
                this.m_uiSystem.RequestNewVisualState(n"inkModalPopupState");
                TimeDilationHelper.SetTimeDilationWithProfile(this.m_player, "radialMenu", true, true);
                PopupStateUtils.SetBackgroundBlur(this, true);
                this.m_audioSystem.Play(n"ui_phone_incoming_call_positive");
                this.m_deliveryListLogicController.Show();
                this.PlayRumble(RumbleStrength.SuperLight, RumbleType.Slow, RumblePosition.Left);
                let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
                let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
                uiBlackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, true, true);
                this.EnableServerInput();
            } else {
                FTLog(s"[MultiplayerGameController] OnDeliveryListSpawned - no logic controller");
            }
        } else {
            FTLog(s"[MultiplayerGameController] OnDeliveryListSpawned - no widget");
        }
    }

    protected cb func OnDeliveryListClosed() -> Bool {
        if IsDefined(this.m_deliveryListLogicController) {
            this.DisableServerInput();
            this.m_audioSystem.Play(n"ui_phone_incoming_call_negative");
            this.m_deliveryListLogicController.Hide();
            this.m_repeatingScrollActionEnabled = false;
            this.PlayRumble(RumbleStrength.SuperLight, RumbleType.Fast, RumblePosition.Left);
            TimeDilationHelper.SetTimeDilationWithProfile(this.m_player, "radialMenu", false, false);
            PopupStateUtils.SetBackgroundBlur(this, false);
            this.m_uiSystem.PopGameContext(UIGameContext.ModalPopup);
            this.m_uiSystem.RestorePreviousVisualState(n"inkModalPopupState");
            this.m_deliveryListLogicController = null;
            (this.GetWidget(n"server_list_slot") as inkCompoundWidget).RemoveChild(this.m_deliveryListWidget);
            this.m_deliveryListWidget = null;
            let blackboardSystem: ref<BlackboardSystem> = GameInstance.GetBlackboardSystem(GetGameInstance());
            let uiBlackboard: ref<IBlackboard> = blackboardSystem.Get(GetAllBlackboardDefs().UIGameData);
            uiBlackboard.SetBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, false, true);
        };
    }

    protected cb func OnCloseDeliveryList(target: wref<inkWidget>) -> Bool {
        // this.CloseContactList();
    }

    protected cb func OnDeliveryListAction(action: ListenerAction, consumer: ListenerActionConsumer) -> Bool {
        let actionName: CName = ListenerAction.GetName(action);
        let actionType: gameinputActionType = ListenerAction.GetType(action);
        if Equals(actionType, gameinputActionType.REPEAT) {
            if !this.m_repeatingScrollActionEnabled {
                return false;
            };
            switch actionName {
                case n"popup_moveDown":
                this.m_deliveryListLogicController.NavigateDown();
                break;
                case n"popup_moveUp":
                this.m_deliveryListLogicController.NavigateUp();
                break;
                case n"popup_moveUp_left_stick_down":
                this.m_deliveryListLogicController.NavigateDown();
                break;
                case n"popup_moveUp_left_stick_up":
                this.m_deliveryListLogicController.NavigateUp();
            };
        } else {
            if Equals(actionType, gameinputActionType.BUTTON_PRESSED) {
                if !this.m_repeatingScrollActionEnabled {
                    this.m_repeatingScrollActionEnabled = true;
                };
                switch actionName {
                case n"popup_moveDown":
                    this.m_deliveryListLogicController.NavigateDown();
                    break;
                case n"popup_moveUp":
                    this.m_deliveryListLogicController.NavigateUp();
                    break;
                case n"popup_moveUp_left_stick_down":
                    this.m_deliveryListLogicController.NavigateDown();
                    break;
                case n"popup_moveUp_left_stick_up":
                    this.m_deliveryListLogicController.NavigateUp();
                    break;
                case n"proceed":
                    // this.AcceptAction();
                    let jobEntry: ref<JobEntry>;
                    if IsDefined(this.m_deliveryListLogicController) {
                        jobEntry = this.m_deliveryListLogicController.GetSelectedDeliveryEntry();
                        if IsDefined(jobEntry) {
                            let delivery = jobEntry as DeliveryEntry;
                            let taxi = jobEntry as TaxiJobEntry;
                            if IsDefined(delivery) {
                                FTLog(s"[MultiplayerGameController] Delivery selected: \(delivery.id)");
                                DeliveryServer.AcceptDelivery(delivery.id);
                            } else if IsDefined(taxi) {
                                FTLog(s"[MultiplayerGameController] Taxi selected: \(taxi.id)");
                                TaxiServer.AcceptJob(taxi.id);
                            }
                            this.m_activeDelivery = true;
                            this.m_player.UnregisterInputListener(this, n"UIJob");
                            this.m_player.RegisterInputListener(this, n"UIShop");
                            this.ShowDeliveryList(false);
                            this.UpdateInputHints();
                        } else {
                            FTLog(s"[MultiplayerGameController] No jobs available");
                        }
                    }
                    break;
                case n"secondaryAction":
                    JobsServer.QuitJob();
                    this.m_activeJob = false;
                    this.UpdateInputHints();
                    this.ShowDeliveryList(false);
                    break;
                case n"OpenPauseMenu":
                    ListenerActionConsumer.DontSendReleaseEvent(consumer);
                    break;
                case n"cancel":
                    this.ShowDeliveryList(false);
                    break;
                case n"popup_moveRight":
                case n"popup_next":
                case n"popup_prior":
                case n"popup_moveLeft":
                    // this.SelectOtherTab();
                    break;
                };
            };
        };
        return true;
    }

    public func OnDeliveryComplete() -> Void {
        this.m_activeDelivery = false;
        this.UpdateInputHints();
        this.m_player.RegisterInputListener(this, n"UIJob");
        this.m_player.UnregisterInputListener(this, n"UIShop");
    }

// Actions

    protected cb func OnAction(action: ListenerAction, consumer: ListenerActionConsumer) -> Bool {
        let actionName: CName = ListenerAction.GetName(action);
        let actionType: gameinputActionType = ListenerAction.GetType(action);
        if !this.m_connectedToServer {
            if !this.m_serverListOpen {
                if Equals(actionName, n"UIConnectToServer") && Equals(actionType, gameinputActionType.BUTTON_HOLD_COMPLETE) {
                    // this.ShowServerList(true);
                    GameInstance.GetNetworkWorldSystem().Connect();
                    return true;
                } else {
                    return false;
                }
            } else {
                if IsDefined(this.m_serverListLogicController) {
                    return this.OnServerListAction(action, consumer);
                };
            }
        } else {
            if IsDefined(this.m_deliveryListLogicController) {
                return this.OnDeliveryListAction(action, consumer);
            }
            if IsDefined(this.m_jobListLogicController) {
                return this.OnJobListAction(action, consumer);
            };
            if Equals(actionName, n"UIDisconnectFromServer") && Equals(actionType, gameinputActionType.BUTTON_HOLD_COMPLETE) {
                GameInstance.GetNetworkWorldSystem().Disconnect();
                return true;
            } else if Equals(actionName, n"UIEmote") && Equals(actionType, gameinputActionType.BUTTON_PRESSED) && !this.m_emoteSelectorOpen {
                this.ShowEmoteSelector(true);
                return true;
            } else if Equals(actionName, n"UIEmote") && Equals(actionType, gameinputActionType.BUTTON_RELEASED) && this.m_emoteSelectorOpen {
                this.ShowEmoteSelector(false);
                return true;
            } else if Equals(actionName, n"UIShop") && Equals(actionType, gameinputActionType.BUTTON_HOLD_COMPLETE) && this.m_activeDelivery {
                DeliveryServer.CancelDelivery();
                // this.m_activeDelivery = false;
                // this.m_player.RegisterInputListener(this, n"UIJob");
                // this.m_player.UnregisterInputListener(this, n"UIShop");
                // this.UpdateInputHints();
                return true;
            } else if Equals(actionName, n"UIJob") && Equals(actionType, gameinputActionType.BUTTON_PRESSED) && !this.m_deliveryListOpen && !this.m_activeDelivery {
                if !this.m_activeJob {
                    this.ShowJobList(true);
                } else {
                    DeliveryServer.LoadDeliveries();
                }
                return true;
            } else {
                // let value = ListenerAction.GetValue(action);
                // let name = ListenerAction.GetName(action);
                // FTLog(s"\(name): \(value)");
                if Equals(ListenerAction.GetName(action), n"mouse_x") || Equals(ListenerAction.GetName(action), n"mouse_y") {
                    this.m_emoteSelector.UpdateAxisIndicator(ListenerAction.GetValue(action), ListenerAction.GetName(action));
                    return true;
                };

                return false;
            }
        }
    }
}
