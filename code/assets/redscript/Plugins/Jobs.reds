module CyberpunkMP.Plugins

import CyberpunkMP.*
import CyberpunkMP.World.*

public class JobEntry {
    let id: Uint32;
    let name: String;
    let description: String;
    let reward: Uint32;
}

public struct JobInfo {
    let id: Uint32;
    let name: String;
    let description: String;
    let reward: Uint32;
}

public native class JobsServer extends ServerRpc {
    public static native func SelectJob(job: CName) -> Void;
    public static native func QuitJob() -> Void;
}

public class JobsClient extends ClientRpc {

    public func SetActiveJob(job: CName) -> Void {
        CMPLog(s"");

        // Set the current active job of the player
        if (Equals(job, n"Delivery Driver")) {
            // Enable the delivery UI
            DeliveryServer.LoadDeliveries();
        } else if (Equals(job, n"Taxi Driver")) {
            TaxiServer.LoadJobs();
        }
        // Handle "None" here
    }
}
