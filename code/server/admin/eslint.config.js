import globals from 'globals';
import react from 'eslint-plugin-react';
import reactHooks from 'eslint-plugin-react-hooks';
import reactRefresh from 'eslint-plugin-react-refresh';
import typescriptEslint from '@typescript-eslint/eslint-plugin';
import typescriptParser from '@typescript-eslint/parser';

export default [
    {
        settings: {
            react: {
                version: 'detect'
            }
        },
        files: ['**/*.{js,jsx,mjs,cjs,ts,tsx}'],
        languageOptions: {
            parser: typescriptParser,
            parserOptions: {
                sourceType: 'module',
                ecmaVersion: 'latest',
                ecmaFeatures: {
                    jsx: true,
                },
            },
            globals: {
                ...globals.browser,
            },
        },
        plugins: {
            react,
            'react-hooks': reactHooks,
            'react-refresh': reactRefresh,
            '@typescript-eslint': typescriptEslint,
        },
        rules: {
            // React rules
            ...react.configs.recommended.rules,
            ...react.configs['jsx-runtime'].rules,

            // React hooks rules
            ...reactHooks.configs.recommended.rules,

            // TypeScript ESLint rules
            ...typescriptEslint.configs.recommended.rules,

            // Custom rules
            'react-refresh/only-export-components': [
                'warn',
                {allowConstantExport: true},
            ],
            'no-restricted-imports': [
                'error',
                {
                    patterns: ['@mui/*/*/*'],
                },
            ],
        },
    },
];
