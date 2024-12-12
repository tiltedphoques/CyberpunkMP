import 'systemjs';
import React, {StrictMode} from 'react';
import {BrowserRouter} from "react-router";
import {createRoot} from 'react-dom/client';
import * as MUI from '@mui/material';
import './index.css';
import App from './App.tsx';
import {useToasts} from "./Toast/ToastReducer.ts";

// NOTE: declare external dependencies globally for plugins.
window.React = React;
window.MUI = MUI;
window.App = {
  useToasts: useToasts
};

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <BrowserRouter>
      <App/>
    </BrowserRouter>
  </StrictMode>,
);
