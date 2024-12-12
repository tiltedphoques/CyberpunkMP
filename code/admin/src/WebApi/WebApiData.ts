import {Layout} from "react-grid-layout";


export interface PluginDto {
  readonly Name: string;
}

export interface PluginManifest {
  name: string;
  url: string;
  author: string;
  version: string;
}

export interface PluginSettings {
  [key: string]: any;
}

export interface PluginWidget {
  component: any;
  layout: Partial<Layout>;
}

export interface PluginPage {

}

export interface PluginModule {
  manifest: PluginManifest;
  settings?: PluginSettings;
  widget?: PluginWidget;
  page?: PluginPage;
}
