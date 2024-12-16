import {configureStore} from "@reduxjs/toolkit";
import {useDispatch, useSelector} from "react-redux";
import pluginReducer from './PluginStore';
import toastReducer from './ToastStore';
import storageReducer from './StorageStore';

const store = configureStore({
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware({
      serializableCheck: {
        ignoredActions: [
          'plugin/loadPlugins', 'plugin/updatePlugin',
        ],
        ignoredPaths: ['plugin.plugins'],
      },
    }),
  reducer: {
    plugin: pluginReducer,
    toast: toastReducer,
    storage: storageReducer,
  },
});

export default store;

export type AppStore = ReturnType<typeof store.getState>;
export type AppDispatch = typeof store.dispatch;

export const useAppDispatch = useDispatch.withTypes<AppDispatch>();
export const useAppSelector = useSelector.withTypes<AppStore>();
