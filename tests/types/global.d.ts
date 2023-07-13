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

    function require(id: 'tin:test'): {
        expect: (actual: any) => {
            toEqual: (expected: any) => void;
        };
        describe: (name: string, fn: () => void) => void;
        test: (name: string, fn: () => void) => void;
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

