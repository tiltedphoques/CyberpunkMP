using CyberpunkSdk;
using EmbedIO;
using EmbedIO.Routing;

namespace EmoteSystem;

public record EmoteDto(string Username, string Emote);

internal class EmoteController : PluginWebApiController
{
    private readonly Plugin _plugin;

    internal EmoteController(Plugin plugin)
    {
        _plugin = plugin;
    }

    [Route(HttpVerbs.Get, "/")]
    public EmoteDto GetLastEmote()
    {
        var lastEmote = _plugin.LastEmote;

        if (lastEmote == null)
        {
            throw HttpException.NotFound();
        }

        return lastEmote;
    }
}