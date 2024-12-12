import {PluginDto, PluginModule} from "./WebApiData.ts";

export class WebApiError extends Error {
  constructor(readonly type: string) {
    super();
  }
}

export class WebApiClient {
  public static async getPlugins(): Promise<PluginModule[]> {
    const response: Response = await fetch('/api/v1/plugins');

    if (!response.ok) {
      throw new WebApiError('plugins');
    }
    const plugins: PluginDto[] = await response.json() as PluginDto[];

    return plugins.map(plugin => {
      return {
        manifest: {
          name: plugin.Name,
          url: `/api/v1/plugins/${plugin.Name}`,
          author: '',
          version: '',
        }
      };
    });
  }

  public static async getWidget(name: string): Promise<PluginModule | undefined> {
    const url: string = `/api/v1/plugins/${name}/assets/widget.umd.js`;

    try {
      return await System.import(url) as PluginModule;
    } catch (error) {
      console.error(error);
    }
  }
}
