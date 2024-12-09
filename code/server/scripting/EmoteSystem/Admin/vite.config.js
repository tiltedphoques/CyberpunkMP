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
            name: 'widget',
            entry: './index.ts',
            formats: ['umd'],
            fileName: 'widget',
        },
        rollupOptions: {
            external: [
                'react',
                '@mui/material',
            ],
            output: {
                name: 'Widget',
                globals: {
                    'react': 'React',
                    '@mui/material': 'MaterialUI',
                }
            }
        }
    },
});
