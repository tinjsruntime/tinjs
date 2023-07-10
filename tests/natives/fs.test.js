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
    test('readFile()', async () => {
        const content = fs.readFile(docPath('hello_world.txt'));
        expect(content).toEqual('Hello, World! xD');
    });
})