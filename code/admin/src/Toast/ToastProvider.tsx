import {Alert, Snackbar} from "@mui/material";
import {ActionDispatch, createContext, ReactElement, useContext, useEffect, useReducer, useState} from "react";
import {equalsToast, Toast, ToastAction, ToastState} from "./ToastReducer.ts";

interface ToastProviderProps {
  readonly children: ReactElement | Array<ReactElement>;
}

export function useToasts() {
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

const toastReducer = (state: ToastState, action: ToastAction): ToastState => {
  if (action.type === 'open') {
    return queueToast(state, action.toast);
  } else if (action.type === 'close') {
    return closeToast(state);
  }
  throw new Error(`ToastProvider: unknown action "${action.type}".`);
};

const queueToast = (state: ToastState, toast?: Toast): ToastState => {
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

const closeToast = (state: ToastState) => {
  let toasts: Toast[] = state.toasts;
  let show: Toast | undefined = toasts.splice(0, 1)[0];

  return {
    toast: show,
    toasts: [...toasts]
  };
}

const toastState: ToastState = {
  toast: undefined,
  toasts: []
};

const ToastContext = createContext<ToastState | null>(null);
const ToastDispatchContext = createContext<ActionDispatch<[action: ToastAction]> | null>(null);

export default function ToastProvider({children}: ToastProviderProps) {
  const [state, dispatch] = useReducer(toastReducer, toastState);
  const [show, setShow] = useState<boolean>(false);
  const [toast, setToast] = useState<Toast>({message: '', duration: 4000});

  useEffect(() => {
    if (!state.toast) {
      return;
    }
    setToast(state.toast);
    setShow(true);
  }, [state]);

  const handleClose = () => {
    setShow(false);
    // NOTE: wait for animation to finish.
    setTimeout(() => {
      dispatch({type: 'close'});
    }, 500);
  };

  return (
    <ToastContext.Provider value={state}>
      <ToastDispatchContext.Provider value={dispatch}>
        <Snackbar
          open={show}
          autoHideDuration={toast.duration}
          onClose={handleClose}>
          <Alert severity={toast.style}
                 variant="filled"
                 sx={{width: '100%'}}>
            {toast.message}
          </Alert>
        </Snackbar>
        {children}
      </ToastDispatchContext.Provider>
    </ToastContext.Provider>
  );
}
