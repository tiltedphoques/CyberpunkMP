import {Widget} from './src/widget';
import {Layout as RGLLayout} from 'react-grid-layout';

const Layout: Partial<RGLLayout> = {
    w: 3,
    h: 2,
    minW: 2,
    minH: 2,
    maxW: 4,
    maxH: 3,
};

const Plugin = {
    manifest: {
        name: 'Emote',
        author: 'Rayshader',
        version: '1.0.0',
    },
    widget: {
        component: Widget,
        layout: Layout
    },
    page: undefined
};

export default Plugin;