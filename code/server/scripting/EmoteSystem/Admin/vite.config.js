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
            //            with other plugins. You must use the name of the
            //            plugin `EmoteSystem` without its suffix `System`.
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
                //'@mui/icons-material',
                //'@mdi/react',
                //'@mdi/js',
                'http-status-codes',
                'App'
            ],
            output: {
                globals: {
                    'react': 'React',
                    //'react-redux': 'Redux',
                    '@mui/material': 'MUI',
                    //'@mui/icons-material': 'MUIIcons',
                    //'@mdi/react': 'MDI',
                    //'@mdi/js': 'MDIIcons',
                    'http-status-codes': 'StatusCodes',
                    'App': 'App',
                }
            }
        }
    },
});
