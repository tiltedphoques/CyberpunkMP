import React, {StrictMode} from 'react';
import * as MaterialUI from '@mui/material';
import {createRoot} from 'react-dom/client';
import './index.css';
import App from './App.tsx';
import {BrowserRouter} from "react-router";
import 'systemjs';

window.React = React;
// @ts-expect-error should declare in global
window.MaterialUI = MaterialUI;

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <BrowserRouter>
      <App/>
    </BrowserRouter>
  </StrictMode>,
);
