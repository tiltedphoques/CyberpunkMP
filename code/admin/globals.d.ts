import {App} from "./api";

declare global {
  interface Window {
    MUI: typeof import('@mui/material');
    App: App;
  }
}

export {};
