import 'systemjs';
import React, {StrictMode} from 'react';
import {BrowserRouter} from "react-router";
import {createRoot} from 'react-dom/client';
import * as MUI from '@mui/material';
import {Provider} from "react-redux";
import './index.css';
import '../node_modules/react-grid-layout/css/styles.css';
import '../node_modules/react-resizable/css/styles.css';
import App from './App.tsx';
import store from './Stores/AppStore';
import {useToasts} from "./Stores/ToastStore.ts";

// NOTE: declare external dependencies globally for plugins.
window.React = React;
window.MUI = MUI;
window.App = {
  useToasts: useToasts
};

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <BrowserRouter>
      <Provider store={store}>
        <App/>
      </Provider>
    </BrowserRouter>
  </StrictMode>,
);