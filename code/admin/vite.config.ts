import {defineConfig} from 'vite';
import react from '@vitejs/plugin-react';

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    react()
  ],
  server: {
    proxy: {
      '/api': 'http://localhost:11778'
    },
  },
  optimizeDeps: {
    include: [
      'react',
      '@mui/material'
    ]
  },
  build: {
    rollupOptions: {
      external: [
        'react',
        '@mui/material'
      ],
      output: {
        globals: {
          'react': 'React',
          '@mui/material': 'MUI',
        }
      }
    }
  }
});
