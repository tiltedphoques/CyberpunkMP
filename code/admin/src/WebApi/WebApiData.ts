import {ReactElement} from "react";
import Module = System.Module;

export interface PluginDto {
  readonly Name: string;
}

export interface PluginData {
  name: string;
  widget?: WidgetData;
  settings?: PluginSettings;
}

export interface PluginSettings {
  [key: string]: any;
}

export interface WidgetData {
  url: string;
  name: string;
  module?: Module;
  element?: ReactElement;
}
