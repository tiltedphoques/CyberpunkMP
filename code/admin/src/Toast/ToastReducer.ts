import {ActionDispatch, createContext, useContext, useReducer} from "react";
import {equalsToast, ShowToastCallback, Toast, ToastAction, ToastState} from "./Toast.ts";

export const ToastContext = createContext<ToastState | null>(null);
export const ToastDispatchContext = createContext<ActionDispatch<[action: ToastAction]> | null>(null);

export function useToasts(): ShowToastCallback {
  const dispatch = useContext(ToastDispatchContext);

  return (toast: Toast) => {
    dispatch!({
      type: 'open',
      toast: {
        ...toast,
        style: toast.style ?? 'info',
        duration: toast.duration ?? 4000,
      }
    });
  };
}

export function reduceToasts() {
  return useReducer(toastReducer, toastState);
}

const toastState: ToastState = {
  toast: undefined,
  toasts: []
};

function toastReducer(state: ToastState, action: ToastAction): ToastState {
  if (action.type === 'open') {
    return queueToast(state, action.toast);
  } else if (action.type === 'close') {
    return closeToast(state);
  }
  throw new Error(`ToastProvider: unknown action "${action.type}".`);
}

function queueToast(state: ToastState, toast?: Toast): ToastState {
  if (!toast) {
    return state;
  }
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
}

function closeToast(state: ToastState) {
  let toasts: Toast[] = state.toasts;
  let toast: Toast | undefined = toasts.splice(0, 1)[0];

  return {
    toast: toast,
    toasts: [...toasts]
  };
}
