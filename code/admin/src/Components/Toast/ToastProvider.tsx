import {Alert, Snackbar} from "@mui/material";
import {useEffect, useState} from "react";
import {useAppDispatch, useAppSelector} from "../../Stores/AppStore.ts";
import {closeToast, ToastState} from "../../Stores/ToastStore.ts";
import {Toast} from "../../Data/ToastData.ts";

export default function ToastProvider() {
  const state: ToastState = useAppSelector(state => state.toast);
  const dispatch = useAppDispatch();
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
      dispatch(closeToast());
    }, 500);
  };

  return (
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
  );
}
