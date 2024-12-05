using EmbedIO;
using EmbedIO.WebApi;

namespace CyberpunkSdk;

public interface IWebApiHook
{
    public Func<PluginWebApiController> BuildController();
}

public class PluginWebApiController : WebApiController
{
    public Func<T> Build<T>() where T : WebApiController => () => this as T;
}