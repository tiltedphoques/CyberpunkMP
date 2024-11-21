local empty = nil
local activeJob = empty
local activeDelivery = nil
local availableDeliveries = {}
local mappinID = nil

registerForEvent('onInit', function()

    empty = ToCName{ hash_lo = 0x00000000, hash_hi = 0x00000000 }
    activeJob = empty

    Override('CyberpunkMP.Plugins.DeliveryClient', 'ReceiveDelivery', function(self, item, count, wrappedMethod)

    end)

    Override('CyberpunkMP.Plugins.DeliveryClient', 'StartDelivery', function(self, location, wrappedMethod)
        activeDelivery = location
        local mappinData = MappinData.new()
        mappinData.mappinType = TweakDBID.new("Mappins.QuestStaticMappinDefinition")
        mappinData.variant = 'DefaultQuestVariant'
        mappinData.active = true;
        mappinID = Game.GetMappinSystem():RegisterMappin(mappinData, location)
    end)

    Override('CyberpunkMP.Plugins.DeliveryClient', 'EndDelivery', function(self, success, reward, wrappedMethod)
        activeDelivery = nil
        if success then
            Game.AddToInventory("Items.money", reward)
        end
        if mappinID ~= nil then
            Game.GetMappinSystem():UnregisterMappin(mappinID)
            mappinID = nil
        end
    end)

    Override('CyberpunkMP.Plugins.DeliveryClient', 'LoadItems', function(self, items, wrappedMethod)
        
    end)

    Override('CyberpunkMP.Plugins.DeliveryClient', 'LoadDeliveries', function(self, deliveries, wrappedMethod)
        availableDeliveries = deliveries
    end)

    Override('CyberpunkMP.Plugins.JobsClient', 'SetActiveJob', function(self, job, wrappedMethod)
        activeJob = job
    end)

end)

local isOverlayVisible = false

-- onOverlayOpen
registerForEvent('onOverlayOpen', function()
    isOverlayVisible = true
end)

-- onOverlayClose
registerForEvent('onOverlayClose', function()
    isOverlayVisible = false
end)

function drawJob()
    if ImGui.Begin('Jobs', ImGuiWindowFlags.AlwaysAutoResize) then
        ImGui.Text('Please select a Job')
        if ImGui.Button("Delivery Driver") then
            GetSingleton("CyberpunkMP.Plugins.JobsServer").SelectJob("Delivery Driver")
        end
    end
    ImGui.End()
end

function drawDelivery()
    if ImGui.Begin('Deliveries', ImGuiWindowFlags.AlwaysAutoResize) then
        if activeDelivery == nil then
            ImGui.Text('Please select a delivery')
            if ImGui.Button("Refresh") then
                GetSingleton("CyberpunkMP.Plugins.DeliveryServer").LoadDeliveries()
            end
            if ImGui.BeginTable("Available deliveries", 3) then
                ImGui.TableNextColumn()
                ImGui.Text("Item")
                ImGui.TableNextColumn()
                ImGui.Text("Reward")
                for i, delivery in ipairs(availableDeliveries) do
                    ImGui.TableNextRow()
                    ImGui.TableNextColumn()
                    ImGui.Text(delivery.item.value)
                    ImGui.TableNextColumn()
                    ImGui.Text(tostring(delivery.reward))
                    ImGui.TableNextColumn()
                    if ImGui.Button("Accept##" .. tostring(delivery.id)) then
                        GetSingleton("CyberpunkMP.Plugins.DeliveryServer").AcceptDelivery(delivery.id)
                    end
                end

                ImGui.EndTable()
            end
        else
            ImGui.Text('Delivery active.')
            if ImGui.Button("Abort") then
                GetSingleton("CyberpunkMP.Plugins.DeliveryServer").CancelDelivery()
                if mappinID ~= nil then
                    Game.GetMappinSystem():UnregisterMappin(mappinID)
                    mappinID = nil
                end
            end
        end

        if ImGui.Button("Quit Job") then
            GetSingleton("CyberpunkMP.Plugins.JobsServer").QuitJob()
            if mappinID ~= nil then
                Game.GetMappinSystem():UnregisterMappin(mappinID)
                mappinID = nil
            end
            activeJob = empty
        end
    end
    ImGui.End()
end

-- onDraw
-- this event is triggered continuously
registerForEvent('onDraw', function()
    
    -- bail early if overlay is not open
    if not isOverlayVisible then
        return
    end
    
    -- draw ImGui window
    if activeJob == empty then
        drawJob()
    else
        drawDelivery()
    end
end)