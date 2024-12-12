export class AppRepository {
  public static get<T = any>(key: string): T | undefined {
    if (!('localStorage' in window)) {
      throw new Error('localStorage is not supported on this browser.');
    }
    const data: string | null = localStorage.getItem(key);

    if (!data) {
      return;
    }
    return JSON.parse(data) as T;
  }

  public static set<T = any>(key: string, value: T): void {
    if (!('localStorage' in window)) {
      throw new Error('localStorage is not supported on this browser.');
    }
    localStorage.setItem(key, JSON.stringify(value));
  }
}
