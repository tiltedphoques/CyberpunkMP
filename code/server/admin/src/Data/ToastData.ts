export type ToastStyle = 'info' | 'success' | 'warning' | 'error';

export type ShowToastCallback = (toast: Toast) => void;

export interface Toast {
  message: string;
  style?: ToastStyle;
  duration?: number;
  onClosed?: () => void;
}

export function equalsToast(a?: Toast, b?: Toast): boolean {
  return a?.style === b?.style &&
    a?.duration === b?.duration &&
    a?.message === b?.message &&
    a?.onClosed === b?.onClosed;
}
