export type ToastStyle = 'info' | 'success' | 'warning' | 'error';

export interface Toast {
  readonly message: string;
  readonly style?: ToastStyle;
  readonly duration?: number;
  readonly onClosed?: () => void;
}

export interface ToastState {
  readonly toast?: Toast;
  readonly toasts: Toast[];
}

export interface ToastAction {
  readonly type: 'open' | 'close';
  readonly toast?: Toast;
}

export function equalsToast(a?: Toast, b?: Toast): boolean {
  return a?.style === b?.style &&
    a?.duration === b?.duration &&
    a?.message === b?.message &&
    a?.onClosed === b?.onClosed;
}
