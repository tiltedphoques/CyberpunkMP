import {defineConfig} from 'vite';
import react from '@vitejs/plugin-react';

export default defineConfig({
    define: {
        'process.env.NODE_ENV': '"production"',
    },
    plugins: [
        react(),
    ],
    build: {
        lib: {
            // IMPORTANT: this name must be unique, or it will conflict
            //            with other plugins. We recommend to use the name
            //            of the plugin `EmoteSystem` without its suffix
            //            `System`.
            name: 'Emote',
            entry: './index.ts',
            formats: ['umd'],
            fileName: 'widget',
        },
        rollupOptions: {
            external: [
                'react',
                //'react-redux',
                '@mui/material',
                //'@mdi/react',
                'App'
            ],
            output: {
                globals: {
                    'react': 'React',
                    //'react-redux': 'Redux',
                    '@mui/material': 'MUI',
                    //'@mdi/react': 'MDI',
                    'App': 'App',
                }
            }
        }
    },
});
