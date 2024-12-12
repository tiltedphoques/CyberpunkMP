import {AppRepository} from "./AppRepository.ts";
import {Layout} from "react-grid-layout";

export class AppService {
  public static loadLayout(): Layout[] {
    return AppRepository.get<Layout[]>('dashboard.layout') ?? [];
  }

  public static saveLayout(layout: Layout[]): void {
    AppRepository.set('dashboard.layout', layout);
  }
}
