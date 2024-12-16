import {createSlice, PayloadAction} from "@reduxjs/toolkit";
import {Layout} from "react-grid-layout";

export interface StorageState {
  layouts: Layout[];
}

const initialState: StorageState = {
  layouts: [],
};

function read<T = any>(key: string): T | undefined {
  if (!('localStorage' in window)) {
    throw new Error('localStorage is not supported on this browser.');
  }
  const data: string | null = localStorage.getItem(key);

  if (!data) {
    return;
  }
  return JSON.parse(data) as T;
}

function write<T = any>(key: string, value: T): void {
  if (!('localStorage' in window)) {
    throw new Error('localStorage is not supported on this browser.');
  }
  localStorage.setItem(key, JSON.stringify(value));
}

export const storageSlice = createSlice({
  name: 'storage',
  initialState,
  reducers: {
    loadLayouts: (state) => {
      state.layouts = read<Layout[]>('dashboard.layout') ?? [];
    },
    saveLayouts: (state, action: PayloadAction<Layout[]>) => {
      write('dashboard.layout', action.payload);
      state.layouts = action.payload;
    }
  }
});

export const {loadLayouts, saveLayouts} = storageSlice.actions;
export default storageSlice.reducer;
