export {};

declare global {
    var __execcwd: string;
    var __filename: string;
    var __dirname: string;

    interface Array<T> {
        random(): T;
        first(): T;
        last(): T;
    }

    var exports: any;

    function require(id: 'tin:test'): {
        expect: (actual: any) => {
            toEqual: (expected: any) => void;
        };
        describe: (name: string, fn: () => void) => void;
        test: (name: string, fn: () => void) => void;
    }

    type serverOn = (path: string, fn: (req: {
        method: 'GET' | 'POST' | 'PUT' | 'DELETE' | 'PATCH',
        body: string,
        params: Record<string, string>,
        version: string,
        path: string
    }) => {
        content: any,
        type: 'json' | 'text' | 'html' | string,
    }) => void;

    function require(id: 'tin:http'): {
        createServer: () => {
            listen: (port: number) => void;
            get: serverOn;
            post: serverOn;
            put: serverOn;
            delete: serverOn;
            patch: serverOn;
        }
    }

    function fetch(url: string, options?: {
        method?: 'GET' | 'POST',
        body?: string,
        headers?: any
    }): {
        status: number,
        response: string,
        json: () => any
    }

    function require(id: string): any;
    function print(...args: any[]): void;
    function println(...args: any[]): void;
}

