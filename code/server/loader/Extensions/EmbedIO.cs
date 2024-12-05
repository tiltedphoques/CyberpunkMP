using System.Linq.Expressions;
using System.Reflection;
using CyberpunkSdk;
using EmbedIO.WebApi;

namespace Server.Loader.Extensions;

public static class EmbedIoExtensions
{
    public static WebApiModule WithController(this WebApiModule webApiModule, PluginWebApiController controller)
    {
        Delegate factoryFunc = Expression
            .Lambda(
                Expression.Call(
                    Expression.Constant(controller),
                    typeof(PluginWebApiController)
                        .GetMethod("Build")!
                        .MakeGenericMethod(controller.GetType())
                    )
                )
            .Compile();
        return (WebApiModule)typeof(WebApiModuleExtensions)
            .GetMethods(BindingFlags.Static | BindingFlags.Public)
            .Single(mi => mi.IsGenericMethod & mi.Name == "WithController" && mi.GetParameters().Length == 2)
            .MakeGenericMethod(controller.GetType())
            .Invoke(null, new object[] { webApiModule, factoryFunc.DynamicInvoke(Array.Empty<object>()) });
    }
}