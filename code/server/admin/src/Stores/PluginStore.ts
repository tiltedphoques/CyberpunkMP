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

export const pluginSlice = createSlice({
  name: 'plugin',
  initialState,
  reducers: {
    loadPlugins: (state, action: PayloadAction<PluginModule[]>) => {
      state.plugins = action.payload;
      state.isLoaded = true;
    },
  }
});

export const {loadPlugins} = pluginSlice.actions;
export default pluginSlice.reducer;
