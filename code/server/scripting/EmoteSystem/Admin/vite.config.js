import {defineConfig} from 'vite';
import react from '@vitejs/plugin-react';

export default defineConfig({
    build: {
        target: 'modules',
        lib: {
            name: 'emote',
            entry: './widget.tsx',
            formats: ['es'],
            fileName: 'widget',
        },
        rollupOptions: {
            // NOTE: Exclude dependencies already declared in Admin webapp.
            //       This will reduce the bundle of your widget with only the
            //       minimum required.
            external: [
                '@emotion/react',
                '@emotion/styled',
                '@mui/icons-material',
                '@mui/material',
                'react',
                'react-dom'
            ],
        },
    },
    plugins: [react()],
});
