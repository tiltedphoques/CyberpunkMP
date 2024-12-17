import {PluginDto, PluginModule} from "../Data/PluginData.ts";

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
          url: `/api/v1/plugins/${plugin.Name.toLowerCase()}`,
          author: '',
          version: '',
        }
      };
    });
  }

  public static async getWidget(name: string): Promise<Blob | undefined> {
    const url: string = `/api/v1/plugins/${name.toLowerCase()}/assets/widget.umd.js`;

    try {
      const response: Response = await fetch(url);

      return await response.blob();
    } catch (error) {
      console.error(error);
    }
  }

  public static async getWidgetETag(name: string, etag?: string): Promise<string | undefined> {
    return await this.getETag(`/api/v1/plugins/${name.toLowerCase()}/assets/widget.umd.js`, etag);
  }

  private static async getETag(url: string, etag?: string): Promise<string | undefined> {
    const headers: Record<string, string> = {};

    if (etag) {
      headers['ETag'] = etag;
    }
    const response: Response = await fetch(url, {method: 'HEAD', headers});

    if (!response.ok) {
      return;
    }
    return response.headers.get('ETag') ?? undefined;
  }
}
