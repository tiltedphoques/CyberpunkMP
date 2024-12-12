import {PluginModule} from "./WebApiData.ts";
import {WebApiClient} from "./WebApiClient.ts";

export class WebApiService {
  public static async getPlugins(): Promise<PluginModule[]> {
    const plugins: PluginModule[] = await WebApiClient.getPlugins();

    for (const plugin of plugins) {
      const module: PluginModule | undefined = await WebApiClient.getWidget(plugin.manifest.name);

      if (module) {
        plugin.manifest = {...plugin.manifest, ...module.manifest};
        plugin.settings = module.settings;
        plugin.widget = module.widget;
        plugin.page = module.page;
      }
    }
    return plugins;
  }
}
