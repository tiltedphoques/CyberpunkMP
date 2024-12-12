import {PluginData, PluginDto, WidgetData} from "./WebApiData.ts";
import Module = System.Module;

export class WebApiError extends Error {
  constructor(readonly type: string) {
    super();
  }
}

export class WebApiClient {
  public static async getPlugins(): Promise<PluginData[]> {
    const response: Response = await fetch('/api/v1/plugins');

    if (!response.ok) {
      throw new WebApiError('plugins');
    }
    const plugins: PluginDto[] = await response.json() as PluginDto[];

    return plugins.map(plugin => {
      return {
        name: plugin.Name
      };
    });
  }

  public static async getWidget(name: string): Promise<WidgetData | undefined> {
    const url: string = `/api/v1/plugins/${name}/assets/widget.umd.js`;

    try {
      const module: Module = await System.import(url);

      return {
        url: url,
        name: name,
        module: module
      };
    } catch (error) {
      console.error(error);
    }
  }
}
