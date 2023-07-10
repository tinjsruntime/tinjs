const { describe, test, expect } = require('tin:test');
const fs = require('tin:fs');

/**
 * 
 * @param {string} val Path to be added to the current working directory
 * @returns {string}
 */
function path(val) {
    return __execcwd + val;
}

/**
 * 
 * @param {string} val Path to be added to the current working directory and the tests/docs directory
 * @returns {string}
 */
function docPath(val) {
    return path('/tests/docs/' + val);
}

describe('fs', () => {
    const testString = 'Hello, World!';
    test('readFile()', () => {
        const content = fs.readFile(docPath('hello_world.txt'));
        expect(content).toEqual('Hello, World! xD');
    });

    test('writeFile()', () => {
        fs.writeFile(docPath('tmp/hello_world.txt'), testString);
        expect(fs.readFile(docPath('tmp/hello_world.txt'))).toEqual(testString);
    });

    test('writeFile() - File already exists', () => {
        fs.writeFile(docPath('tmp/hello_world.txt'), testString + "X");
        expect(fs.readFile(docPath('tmp/hello_world.txt'))).toEqual(testString + "X");
        fs.writeFile(docPath('tmp/hello_world.txt'), testString);
    });

    test('appendFile()', () => {
        fs.appendFile(docPath('tmp/hello_world.txt'), "\n" + testString);
        expect(fs.readFile(docPath('tmp/hello_world.txt'))).toEqual(testString + "\n" + testString);
    });

    test('exists()', () => {
        expect(fs.exists(docPath('tmp/hello_world.txt'))).toEqual(true);
    });
    
    test('rmFile()', () => {
        fs.rmFile(docPath('tmp/hello_world.txt'));
        expect(fs.exists(docPath('tmp/hello_world.txt'))).toEqual(false);
    });
})