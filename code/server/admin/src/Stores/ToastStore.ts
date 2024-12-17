import {createSlice, PayloadAction} from "@reduxjs/toolkit";
import {equalsToast, ShowToastCallback, Toast} from "../Data/ToastData.ts";
import {useAppDispatch} from "./AppStore.ts";

export interface ToastState {
  toast?: Toast;
  toasts: Toast[];
}

const initialState: ToastState = {
  toast: undefined,
  toasts: [],
};

export function useToasts(): ShowToastCallback {
  const dispatch = useAppDispatch();

  return (toast: Toast) => {
    dispatch(showToast(toast));
  };
}

export const toastSlice = createSlice({
  name: 'toast',
  initialState,
  reducers: {
    showToast: (state, action: PayloadAction<Toast>) => {
      const toast: Toast = action.payload;

      toast.style ??= 'info';
      toast.duration ??= 4000;
      const lastToast: Toast | undefined = state.toasts[state.toasts.length - 1] ?? state.toast;

      if (equalsToast(lastToast, toast)) {
        return state;
      }
      if (state.toast) {
        return {
          toast: state.toast,
          toasts: [...state.toasts, toast]
        }
      }
      return {
        toast: toast,
        toasts: []
      };
    },
    closeToast: (state) => {
      let toasts: Toast[] = state.toasts;
      let toast: Toast | undefined = toasts.splice(0, 1)[0];

      return {
        toast: toast,
        toasts: [...toasts]
      };
    }
  }
});

export const {showToast, closeToast} = toastSlice.actions;
export default toastSlice.reducer;
