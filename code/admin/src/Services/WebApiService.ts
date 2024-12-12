import {createElement, ReactElement} from "react";
import Module = System.Module;

export interface PluginDto {
  readonly Name: string;
}

export interface PluginData {
  readonly name: string;
  readonly widget?: WidgetData;
  readonly settings?: PluginSettings;
}

export interface PluginSettings {
  [key: string]: any;
}

export interface WidgetData {
  readonly url: string;
  readonly name: string;
  readonly element: ReactElement;
}

export class WebApiError extends Error {
  constructor(readonly type: string) {
    super();
  }
}

export interface WebApiService {
  getPlugins(): Promise<PluginData[]>;
  getWidget(name: string): Promise<WidgetData | undefined>;
}

export function useWebApi(): WebApiService {
  return {
    getPlugins: getPlugins,
    getWidget: getWidget,
  };
}

async function getPlugins(): Promise<PluginData[]> {
  const response: Response = await fetch('/api/v1/plugins');

  if (!response.ok) {
    throw new WebApiError('plugins');
  }
  const data: PluginDto[] = await response.json() as PluginDto[];
  const plugins: PluginData[] = [];

  for (const item of data) {
    const widget: WidgetData | undefined = await getWidget(item.Name);

    plugins.push({
      name: item.Name,
      widget: widget,
    });
  }
  return plugins;
}

async function getWidget(name: string): Promise<WidgetData | undefined> {
  const url: string = `/api/v1/plugins/${name}/assets/widget.umd.js`;

  try {
    const module: Module = await System.import(url);
    const element: ReactElement = createElement(module.default);

    return {
      url: url,
      name: name,
      element: element,
    };
  } catch (error) {
    return;
  }
}
