import {Alert, Snackbar} from "@mui/material";
import {ReactElement, useEffect, useState} from "react";
import {Toast} from "./Toast.ts";
import {reduceToasts, ToastContext, ToastDispatchContext} from "./ToastReducer.ts";

interface ToastProviderProps {
  readonly children: ReactElement | Array<ReactElement>;
}

export default function ToastProvider({children}: ToastProviderProps) {
  const [state, dispatch] = reduceToasts();
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
