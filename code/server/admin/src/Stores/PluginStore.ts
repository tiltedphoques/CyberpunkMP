import {createSlice, PayloadAction} from "@reduxjs/toolkit";
import {PluginModule} from "../Data/PluginData.ts";

interface PluginState {
  isLoaded: boolean;
  plugins: PluginModule[];
}

const initialState: PluginState = {
  isLoaded: false,
  plugins: [],
};

function sortPlugins(a: PluginModule, b: PluginModule): number {
  return a.manifest.name.localeCompare(b.manifest.name);
}

export const pluginSlice = createSlice({
  name: 'plugin',
  initialState,
  reducers: {
    loadPlugins: (_state, action: PayloadAction<PluginModule[]>) => {
      return {
        plugins: action.payload.sort(sortPlugins),
        isLoaded: true,
      }
    },
    updatePlugin: (state, action: PayloadAction<PluginModule>) => {
      const plugin: PluginModule = action.payload;
      const plugins: PluginModule[] = state.plugins.filter(item => item.manifest.name !== plugin.manifest.name);

      return {
        plugins: [...plugins, plugin].sort(sortPlugins),
        isLoaded: state.isLoaded
      }
    },
  }
});

export const {loadPlugins, updatePlugin} = pluginSlice.actions;
export default pluginSlice.reducer;
