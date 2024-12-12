import {PluginData} from "./WebApiData.ts";
import {WebApiClient} from "./WebApiClient.ts";

export class WebApiService {
  public static async getPlugins(): Promise<PluginData[]> {
    const plugins: PluginData[] = await WebApiClient.getPlugins();

    for (const plugin of plugins) {
      plugin.widget = await WebApiClient.getWidget(plugin.name);
    }
    return plugins;
  }
}
