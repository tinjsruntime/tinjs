(function () {
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
        const tmpHW = docPath('tmp/hello_world.txt');

        test('readFile()', () => {
            const content = fs.readFile(docPath('hello_world.txt'));
            expect(content).toEqual('Hello, World! xD');
        });

        test('writeFile()', () => {
            fs.writeFile(tmpHW, testString);
            expect(fs.readFile(tmpHW)).toEqual(testString);
        });

        test('writeFile() - File already exists', () => {
            fs.writeFile(tmpHW, testString + "X");
            expect(fs.readFile(tmpHW)).toEqual(testString + "X");
            fs.writeFile(tmpHW, testString);
        });

        test('appendFile()', () => {
            fs.appendFile(tmpHW, "\n" + testString);
            expect(fs.readFile(tmpHW)).toEqual(testString + "\n" + testString);
        });

        test('exists()', () => {
            expect(fs.exists(tmpHW)).toBeTrue();
        });

        test('rmFile() - Cleanup ' + tmpHW, () => {
            fs.rmFile(tmpHW);
            expect(fs.exists(tmpHW)).toEqual(false);
        });
    })

})();