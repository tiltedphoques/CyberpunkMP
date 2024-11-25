// This file is automatically generated

#pragma warning disable CS8618
#pragma warning disable CS0219

using CyberpunkSdk;
using CyberpunkSdk.Types;

using CyberpunkSdk.Systems;

using CyberpunkSdk.Internal;

namespace Cyberpunk.Rpc.Server.Plugins
{
public static partial class TaxiServer
{
	[ServerRpc(Class = 7779180125398187430, Function = 17835077999319062596)]
	public static void CreateJob(ulong playerId, IBuffer data)
	{
		ushort length = 0;
		var location = new Cyberpunk.Rpc.Vector4(data);

		CreateJob_Impl(playerId, location);
	}

	public static partial void CreateJob_Impl(ulong playerId, Cyberpunk.Rpc.Vector4 location);

	[ServerRpc(Class = 7779180125398187430, Function = 14404329119577371264)]
	public static void AcceptJob(ulong playerId, IBuffer data)
	{
		ushort length = 0;
		var id = data.ReadUint32();

		AcceptJob_Impl(playerId, id);
	}

	public static partial void AcceptJob_Impl(ulong playerId, uint id);

	[ServerRpc(Class = 7779180125398187430, Function = 8241328533315033494)]
	public static void RiderReady(ulong playerId, IBuffer data)
	{
		ushort length = 0;

		RiderReady_Impl(playerId);
	}

	public static partial void RiderReady_Impl(ulong playerId);

	[ServerRpc(Class = 7779180125398187430, Function = 17491794941885880434)]
	public static void CancelJob(ulong playerId, IBuffer data)
	{
		ushort length = 0;

		CancelJob_Impl(playerId);
	}

	public static partial void CancelJob_Impl(ulong playerId);

	[ServerRpc(Class = 7779180125398187430, Function = 4392292666206402825)]
	public static void LoadJobs(ulong playerId, IBuffer data)
	{
		ushort length = 0;

		LoadJobs_Impl(playerId);
	}

	public static partial void LoadJobs_Impl(ulong playerId);

}
}