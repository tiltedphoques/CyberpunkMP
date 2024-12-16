import {Layout} from "react-grid-layout";
import {FunctionComponent} from "react";

export interface PluginDto {
  readonly Name: string;
}

export interface PluginManifest {
  name: string;
  url: string;
  author: string;
  version: string;
}

/*
export interface PluginSettings {
  [key: string]: any;
}
*/

export interface PluginWidget {
  component: FunctionComponent<unknown>;
  layout: Partial<Layout>;
}

export type PluginPage = unknown

export interface PluginModule {
  updatedAt?: string;
  manifest: PluginManifest;
  //settings?: PluginSettings;
  widget?: PluginWidget;
  page?: PluginPage;
}
